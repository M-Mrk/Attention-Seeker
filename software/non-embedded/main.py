import sys
import asyncio
from PySide6.QtWidgets import QApplication, QMainWindow, QLabel, QVBoxLayout, QHBoxLayout, QWidget, QComboBox, QPushButton, QTextEdit, QLineEdit, QToolButton
from PySide6.QtCore import Qt, QThread, Signal

import json
import threading
import datetime
import serial
import serial.tools.list_ports
from dbus_next.aio import MessageBus
from dbus_next import MessageType, Message

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("Attention Seeker")

        # Main layout
        layout = QVBoxLayout()

        # Section 1: Dropdown with label and refresh button
        dropdown_label = QLabel("Select Attention-Seeker port:")
        dropdown_label.setAlignment(Qt.AlignmentFlag.AlignLeft)
        self.dropdown = QComboBox()
        self.refresh_button = QPushButton("Refresh")
        self.refresh_button.clicked.connect(self.refresh_ports)

        # Populate the dropdown initially
        self.populate_ports()

        # Create a horizontal layout for the dropdown and refresh button
        dropdown_layout = QHBoxLayout()
        dropdown_layout.addWidget(self.dropdown)
        dropdown_layout.addWidget(self.refresh_button)

        layout.addWidget(dropdown_label)
        layout.addLayout(dropdown_layout)

        # Section 2: Buttons
        self.button1 = QPushButton("Sync Time")
        self.button3 = QPushButton("Clear output")
        self.button1.clicked.connect(self.send_time_sync)
        self.button3.clicked.connect(lambda: self.clear_output())
        layout.addWidget(self.button1)
        layout.addWidget(self.button3)

        # JSON notification inputs
        # Collapsible notification form toggle
        self.toggle_notify_button = QToolButton()
        self.toggle_notify_button.setText("Send test notification")
        self.toggle_notify_button.setCheckable(True)
        self.toggle_notify_button.setChecked(False)
        self.toggle_notify_button.setArrowType(Qt.RightArrow)
        layout.addWidget(self.toggle_notify_button)

        origin_label = QLabel("Origin:")
        self.origin_input = QLineEdit()
        self.origin_input.setPlaceholderText("e.g. AttentionSeeker")

        title_label = QLabel("Title:")
        self.title_input = QLineEdit()
        self.title_input.setPlaceholderText("Notification title")

        body_label = QLabel("Body:")
        self.body_input = QTextEdit()
        self.body_input.setPlaceholderText("Notification body text")

        self.send_json_button = QPushButton("Send JSON Notification")
        self.send_json_button.clicked.connect(self.send_notification_json)

        # Put the form into a container so we can collapse it
        self.notify_form = QWidget()
        form_layout = QVBoxLayout()
        form_layout.setContentsMargins(12, 0, 0, 0)
        form_layout.addWidget(origin_label)
        form_layout.addWidget(self.origin_input)
        form_layout.addWidget(title_label)
        form_layout.addWidget(self.title_input)
        form_layout.addWidget(body_label)
        form_layout.addWidget(self.body_input)
        form_layout.addWidget(self.send_json_button)
        self.notify_form.setLayout(form_layout)
        self.notify_form.setVisible(False)  # collapsed by default
        layout.addWidget(self.notify_form)

        # Toggle behaviour
        def _toggle_notify(checked: bool):
            self.notify_form.setVisible(checked)
            self.toggle_notify_button.setArrowType(Qt.DownArrow if checked else Qt.RightArrow)

        self.toggle_notify_button.toggled.connect(_toggle_notify)

        # Section 3: Text output box
        self.text_output = QTextEdit()
        self.text_output.setReadOnly(True)
        layout.addWidget(self.text_output)

        # Set central widget
        container = QWidget()
        container.setLayout(layout)
        self.setCentralWidget(container)

        # Serial reading thread handle
        self.serial_thread = None

        # System notification listener
        self.notification_listener = NotificationListener()
        self.notification_listener.notification_received.connect(self.handle_system_notification)
        self.notification_listener.error.connect(lambda e: self.update_output(f"Notification listener error: {e}"))
        self.notification_listener.start()

        # React to port selection changes
        self.dropdown.currentIndexChanged.connect(self.on_port_changed)

        # Start reading from the initially selected port (if any)
        self.on_port_changed(self.dropdown.currentIndex())

    def populate_ports(self):
        self.dropdown.clear()
        for port, desc in get_serial_ports():
            self.dropdown.addItem(f"{port} — {desc}", port)

    def refresh_ports(self):
        self.populate_ports()


    def update_output(self, message=None):
        if message is None:
            message = f"Selected: {self.dropdown.currentText()}"
        self.text_output.append(message)

    def clear_output(self):
        self.text_output.clear()

    def send_notification_json(self):
        origin = self.origin_input.text().strip()
        title = self.title_input.text().strip()
        body = self.body_input.toPlainText().strip()

        if not origin:
            self.update_output("Please set origin before sending.")
            return

        payload = {
            "type": "notification",
            "origin": origin,
            "title": title,
            "body": body,
        }

        self._send_payload(payload, "Sent", "Failed to send")

    def send_time_sync(self):
        # send current computer time/date as JSON
        ts = datetime.datetime.now().isoformat()

        payload = {"type": "time_sync", "timestamp": ts}
        self._send_payload(payload, "Sent time sync", "Failed to send time sync")

    def _send_payload(self, payload: dict, success_label: str, failure_label: str):
        try:
            json_str = json.dumps(payload, ensure_ascii=False)
        except Exception as e:
            self.update_output(f"JSON serialization error: {e}")
            return False

        data = (json_str + "\n").encode("utf-8")

        if self.serial_thread is None:
            self.update_output("No serial connection to send to.")
            return False

        try:
            self.serial_thread.write(data)
            self.update_output(f"{success_label}: {json_str}")
            return True
        except Exception as e:
            self.update_output(f"{failure_label}: {e}")
            return False

    def on_port_changed(self, index):
        port = self.dropdown.itemData(index)
        if not port or port == "None found":
            self.stop_serial_reader()
            self.update_output("No serial device selected.")
            return
        self.start_serial_reader(port)

    def start_serial_reader(self, port, baud=115200):
        self.stop_serial_reader()
        self.serial_thread = SerialReader(port, baud)
        self.serial_thread.data_received.connect(lambda s: self.update_output(s))
        self.serial_thread.error.connect(lambda e: self.update_output(f"Serial error: {e}"))
        self.serial_thread.finished.connect(lambda: self.update_output(f"Serial reader stopped for {port}"))
        self.serial_thread.start()
        self.update_output(f"Connected to {port}")

    def stop_serial_reader(self):
        if self.serial_thread is not None:
            try:
                self.serial_thread.stop()
            except Exception:
                pass
            self.serial_thread = None

    def closeEvent(self, event):
        self.stop_serial_reader()
        if self.notification_listener is not None:
            try:
                self.notification_listener.stop()
                self.notification_listener.wait(1500)
            except Exception:
                pass
        super().closeEvent(event)

    def handle_system_notification(self, payload: dict):
        origin = payload.get("origin") or "System"
        title = payload.get("title") or ""
        body = payload.get("body") or ""
        urgency = payload.get("urgency")

        preview = f"[OS Notification] {origin}: {title}"
        if body:
            preview = f"{preview} — {body}"
        if urgency is not None:
            preview = f"{preview} (urgency={urgency})"

        self.update_output(preview)
        self._send_payload(payload, "Forwarded notification", "Failed to forward notification")

def get_serial_ports():
    ports = serial.tools.list_ports.comports()
    if not ports:
        return [("None found", "Try to refresh")]

    def _is_likely_esp32(port) -> bool:
        dev = port.device or ""
        # Linux: ignore onboard UARTs (ttyS*), keep USB/ACM devices
        if dev.startswith("/dev/ttyS"):
            return False
        if "ttyUSB" in dev or "ttyACM" in dev:
            return True
        # VID for Espressif is usually 0x303A when using TinyUSB
        if port.vid and port.vid == 0x303A:
            return True
        # Fallback: match manufacturer/product strings
        m = (port.manufacturer or "").lower()
        prod = (port.product or "").lower()
        return "esp" in m or "tinyusb" in prod or "esp" in prod

    filtered = [p for p in ports if _is_likely_esp32(p)]
    target = filtered if filtered else ports  # show all if filter empty

    out = []
    for p in target:
        desc = p.description or ""
        if p.manufacturer:
            desc = f"{desc} ({p.manufacturer})" if desc else p.manufacturer
        out.append((p.device, desc))

    return out if out else [("None found", "Try to refresh")]


class NotificationListener(QThread):
    notification_received = Signal(dict)
    error = Signal(str)

    def __init__(self):
        super().__init__()
        self._stop_evt = threading.Event()

    def stop(self):
        self._stop_evt.set()

    def run(self):
        try:
            asyncio.run(self._run())
        except Exception as e:
            self.error.emit(str(e))

    async def _run(self):
        try:
            bus = await MessageBus().connect()
        except Exception as e:
            self.error.emit(f"Failed to connect to D-Bus: {e}")
            return

        def _handle_message(message):
            if message.message_type != MessageType.METHOD_CALL:
                return False
            if message.interface != "org.freedesktop.Notifications" or message.member != "Notify":
                return False

            args = message.body or []
            app_name = args[0] if len(args) > 0 else ""
            summary = args[3] if len(args) > 3 else ""
            body = args[4] if len(args) > 4 else ""
            hints = args[6] if len(args) > 6 else {}

            urgency = None
            try:
                if isinstance(hints, dict):
                    urgency = hints.get("urgency")
            except Exception:
                urgency = None

            payload = {
                "type": "notification",
                "origin": app_name or "System",
                "title": summary or "",
                "body": body or "",
            }
            if urgency is not None:
                payload["urgency"] = urgency

            self.notification_received.emit(payload)
            return False

        bus.add_message_handler(_handle_message)

        match_rule = "type='method_call',interface='org.freedesktop.Notifications',member='Notify'"

        async def _add_match(rule: str):
            msg = Message(
                destination="org.freedesktop.DBus",
                path="/org/freedesktop/DBus",
                interface="org.freedesktop.DBus",
                member="AddMatch",
                signature="s",
                body=[rule],
            )
            reply = await bus.call(msg)
            if reply.message_type == MessageType.ERROR:
                raise RuntimeError(reply.body[0] if reply.body else reply.error_name)

        # Try with eavesdrop first (may be blocked by bus policy), then fall back without it
        try:
            await _add_match(match_rule + ",eavesdrop='true'")
        except Exception as e1:
            self.error.emit(f"Notify subscription with eavesdrop failed: {e1}; retrying without eavesdrop")
            try:
                await _add_match(match_rule)
            except Exception as e2:
                self.error.emit(f"Failed to subscribe to notifications: {e2}")
                bus.disconnect()
                return

        try:
            while not self._stop_evt.is_set():
                await asyncio.sleep(0.25)
        finally:
            try:
                bus.disconnect()
            except Exception:
                pass


class SerialReader(QThread):
    data_received = Signal(str)
    error = Signal(str)

    def __init__(self, port, baud=115200):
        super().__init__()
        self.port = port
        self.baud = baud
        self._running = True
        self.ser = None
        self._write_lock = threading.Lock()

    def run(self):
        try:
            self.ser = serial.Serial(self.port, self.baud, timeout=0.5)
        except Exception as e:
            self.error.emit(str(e))
            return

        try:
            while self._running:
                try:
                    line = self.ser.readline()
                except Exception as e:
                    self.error.emit(str(e))
                    break
                if not line:
                    continue
                try:
                    text = line.decode(errors='replace').rstrip('\r\n')
                except Exception:
                    text = repr(line)
                text = "[Device]: " + text
                self.data_received.emit(text)
        finally:
            try:
                if self.ser and self.ser.is_open:
                    self.ser.close()
            except Exception:
                pass

    def stop(self):
        self._running = False
        self.wait(1000)

    def write(self, data: bytes):
        if not isinstance(data, (bytes, bytearray)):
            raise TypeError("data must be bytes")
        with self._write_lock:
            if not self.ser or not self.ser.is_open:
                raise RuntimeError("Serial port not open")
            try:
                self.ser.write(data)
            except Exception as e:
                self.error.emit(str(e))
                raise

if __name__ == "__main__":
    app = QApplication(sys.argv)

    window = MainWindow()
    window.show()

    sys.exit(app.exec())