using System;
using System.IO;
using System.IO.Ports;
using System.Threading;

/*
 * SerialComPort class which handles the SerialPort object for communicating with the Nano and contains structure of packets and commands being sent/received
 */
public class SerialComPort
{
    // Error checking bytes
    private const byte ACK = 0xCC;
    private const byte NAK = 0x33;
    private const byte PACKET_SIZE = 11;

    // Status and Command bytes
    private enum Status : byte { SUCCESS = 0x40, UNKNOWN_CMD, INVALID_CMD, INVALID_ADDR, MEMORY_FAILED, CHECKSUM_INVALID }
    private enum Cmd : byte { CMD_BEGIN = 0x20, PING = CMD_BEGIN, DOWNLOAD, RUN, GET_STATUS, SEND_DATA, RESET, CMD_END }

    // Packet Types
    static byte[] pingPacketChecksumInvalid = { 0x03, 0xee, (byte)Cmd.PING, 0 };
    static byte[] pingPacket = { 0x03, 0x20, (byte)Cmd.PING, 0 };
    static byte[] getStatusPacket = { 0x03, 0x23, (byte)Cmd.GET_STATUS, 0 };
    static byte[] sendDataPacket = { 0x09, 0xBC, 0x24, 0x91, 0xFF, 0x82, 0xFF, 0x87, 0x00, 0 };
    static byte[] runPacket = { 0x03, 0x22, (byte)Cmd.RUN, 0 };

    static bool _continue;
    static bool _run;
    static SerialPort _serialPort;

    static byte[] buffer = new byte[12];
}

/**
 * GetCode()
 * 
 * Usage: Used to convert a .exe file into an array of bytes which can be transmitted as packets to the serial target.
 * 
 * Provided by: 
 * Copyright (C) 2020 by Michel de Champlain
 */
static byte[] GetCode(string exeFilename)
{
    // Read the file into a byte array.
    var fs = new FileStream(exeFilename, FileMode.Open);
    var fileLength = (int)fs.Length;
    var filePgmSize = fileLength - 2;
    var bufferLength = 3 + filePgmSize + 1; // [size|cksm|cmd(3) + pgm(6) + zero(1)]
    var buffer = new byte[bufferLength];

    // Read the first two bytes to skip the size of the executable.
    fs.Read(buffer, 0, 2);

    // Get only the executable code in the buffer starting at index 3.
    fs.Read(buffer, 3, filePgmSize);
    buffer[0] = (byte)bufferLength;
    buffer[2] = (byte)Cmd.SendData;

    byte checksum = buffer[2];
    // Calculate the checksum for range [2..n-1]
    for (int n = 3; n < bufferLength; ++n)
    {
        checksum += buffer[n];
    }
    buffer[1] = checksum;
    return buffer;
}

public static void Main(string[] args)
{
    if(args.Length != 1)
    {
        Console.WriteLine("Must have atleast 1 argument... (Usage: SerialLoader.exe <file.exe>)");
        return;
    }

    string testFileName = args[0];
    var buf = GetCode(exeFilename);
    byte[] sendDataPacketFile = new byte[buf.Length];

    for (int n = 0; n < buf.Length; ++n)
    {
        sendDataPacketFile[n] = buf[n];
    }

    StringComparer stringComparer = StringComparer.OrdinalIgnoreCase;
    Thread readThread = new Thread(ReadByte);

    _serialPort = new SerialPort();
    _serialPort.PortName = "COM11";
    _serialPort.BaudRate = 9600; ;
    _serialPort.Parity = Parity.None;
    _serialPort.DataBits = 8;
    _serialPort.StopBits = StopBits.One;
    _serialPort.Handshake = Handshake.None;
}