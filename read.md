TELETEXTER V1.1
===============


What is Teletexter?
-------------------
TeleTexter is a program/service that allows you to send
text messages or custom text strings to any I2C interface LCD display
connected to a Raspberry Pi over TCP/IP by sending commands.


Command List:
-------------------

Message Commands:
MSG_CREATE "<Your Message>"
    - This command will create a new message and display it on your LCD as:
            Fri, 16:22 <user>:
            Hello World users of TeleTexter!
OK Replies:
    - OK 110 MESSAGE_SET - Message was created in database and displayed!
ERROR Replies:
    - ERROR 300 ACCESS_DENIED - Insufficient access level


MSG_DEL <id>



 Teletexter is a program/service that can send text messages or custom text strings to your I2C LCD Display
 from a TCP/IP connection, it is highly customizable where you can set the layout
 for your messages the way you want. It supports most I2C displays out there.
