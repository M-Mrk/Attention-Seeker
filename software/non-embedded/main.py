import sys
from PySide6.QtWidgets import QApplication, QMainWindow, QLabel, QVBoxLayout, QHBoxLayout, QWidget, QComboBox, QPushButton, QTextEdit
from PySide6.QtCore import Qt

import serial.tools.list_ports

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

        # Section 2: Three buttons
        self.button1 = QPushButton("Sync Time")
        self.button2 = QPushButton("Send test notification")
        self.button3 = QPushButton("Clear output")
        self.button1.clicked.connect(lambda: self.update_output("Syncing time..."))
        self.button2.clicked.connect(lambda: self.update_output("Sending test notification..."))
        self.button3.clicked.connect(lambda: self.clear_output())
        layout.addWidget(self.button1)
        layout.addWidget(self.button2)
        layout.addWidget(self.button3)

        # Section 3: Text output box
        self.text_output = QTextEdit()
        self.text_output.setReadOnly(True)
        layout.addWidget(self.text_output)

        # Set central widget
        container = QWidget()
        container.setLayout(layout)
        self.setCentralWidget(container)

    def populate_ports(self):
        self.dropdown.clear()
        for port, desc in get_serial_ports():
            self.dropdown.addItem(f"{port} — {desc}", port)

    def refresh_ports(self):
        self.populate_ports()
        self.update_output("Ports refreshed.")

    def update_output(self, message=None):
        if message is None:
            message = f"Selected: {self.dropdown.currentText()}"
        self.text_output.append(message)

    def clear_output(self):
        self.text_output.clear()

def get_serial_ports():
    ports = serial.tools.list_ports.comports()
    available_ports = []

    for port in ports:
        if not port.description == "n/a":
            available_ports.append(port)

    output = [(p.device, p.description) for p in available_ports]
    if len(output) < 1:
        return [("None found", "Try to refresh")]
    return output

if __name__ == "__main__":
    # ports = serial.tools.list_ports.comports()
    # for i, port in enumerate(ports):
    #     print(f"{i}:")
    #     for k, v in port.__dict__.items():
    #         print(f"\t{k}: {v}")

    app = QApplication(sys.argv)

    window = MainWindow()
    window.show()

    sys.exit(app.exec())