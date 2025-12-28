import asyncio
from dbus_next.aio import MessageBus
from dbus_next import Message


async def main():
    bus = await MessageBus().connect()

    msg = Message(
        destination="org.freedesktop.Notifications",
        path="/org/freedesktop/Notifications",
        interface="org.freedesktop.Notifications",
        member="Notify",
        signature="susssasa{sv}i",
        body=[
            "AttentionSeekerTest",  # app_name
            0,                       # replaces_id
            "",                     # app_icon
            "Attention Seeker test",  # summary
            "Hello from dbus-next test script",  # body
            [],                      # actions
            {},                      # hints
            5000,                    # expire_timeout ms
        ],
    )

    await bus.call(msg)
    bus.disconnect()


if __name__ == "__main__":
    asyncio.run(main())
