/*
 * SerialLoader.cs - Nicolas Samaha.
 * 
 * This program contains reference code extracted from Michel de Champlain's SerialLoader.cs that was provided.
 */

using System;
using System.IO;
using System.IO.Ports;
using System.Threading;

public class SerialPortCommunication
{
    // Error checking bytes
    private const byte ACK = 0xCC;
    private const byte NAK = 0x33;
    private const byte PACKET_SIZE = 11;

    public static bool _continue;
    public static bool _run;
    public static bool _reading;
    public static SerialPort serialPort;

    // Define Status and Command enums
    private enum Status : byte { SUCCESS = 0x40, UNKNOWN_CMD, INVALID_CMD, INVALID_ADDR, MEMORY_FAILED, CHECKSUM_INVALID }
    private enum Cmd : byte { CMD_BEGIN = 0x20, PING = CMD_BEGIN, DOWNLOAD, RUN, GET_STATUS, SEND_DATA, RESET, CMD_END }

    // Define packet structures to be sent
    public static byte[] pingPacketChecksumInvalid = { 0x03, 0xee, (byte)Cmd.PING, 0 };
    public static byte[] pingPacket = { 0x03, 0x20, (byte)Cmd.PING, 0 };
    public static byte[] getStatusPacket = { 0x03, 0x23, (byte)Cmd.GET_STATUS, 0 };
    //public static byte[] sendDataPacketFile = { 0x09, 0xBC, 0x24, 0x91, 0xFF, 0x82, 0xFF, 0x87, 0x00, 0 };
    public static byte[] runPacket = { 0x03, 0x22, (byte)Cmd.RUN, 0 };

    // Define read buffer
    static byte[] buffer = new byte[12];


     /**
     * GetCode()
     * 
     * Usage: Used to convert a .exe file into an array of bytes which can be transmitted as packets to the serial target.
     * 
     * Copyright (C) 2020 by Michel de Champlain
     */
    public static byte[] GetCode(String exeFilename)
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
        buffer[2] = (byte)Cmd.SEND_DATA;

        byte checksum = buffer[2];
        // Calculate the checksum for range [2..n-1]
        for (int n = 3; n < bufferLength; ++n)
        {
            checksum += buffer[n];
        }
        buffer[1] = checksum;
        return buffer;
    }

    /**
     * ReadByte()
     * 
     * Usage: Simultaneously read bytes being received by the Nano during runtime
     * 
     * Copyright (C) 2020 by Michel de Champlain
     */
    public static void ReadByte()
    {
        while (_continue)
        {
            try
            {
                int size = serialPort.Read(buffer, 0, 1);
                if (buffer[0] != 0)
                {
                    do
                    {
                        if (!_run && (buffer[0] == ACK))
                        {
                            size = serialPort.Read(buffer, 0, 1); // read the zero
                            Console.Write("Ack from target\n$ ");
                            break;
                        }

                        if (_run && (buffer[0] == ACK))
                        {
                            size = serialPort.Read(buffer, 0, 1); // read the zero
                            Console.Write("Ack from target. Run!\n");
                            break;
                        }
                        size = serialPort.Read(buffer, 0, 1);
                    } while ((buffer[0] != 0));
                }

                if (_run)
                {
                    while (true)
                    {
                        size = serialPort.Read(buffer, 0, 1);

                        if (buffer[0] == ACK)
                        {
                            size = serialPort.Read(buffer, 0, 1); // read the zero
                            break;
                        }
                        Console.Write((char)buffer[0]);
                    }
                    _run = false;
                    Console.Write("$ ");
                }
            }
            catch (TimeoutException) { }
        }
    }

    public static void Main(string[] args)
    {
        // Check if run with a file argument.
        if (args.Length != 1)
        {
            Console.WriteLine("Must have atleast 1 argument... (Usage: SerialLoader.exe <file.exe>)");
            return;
        }

        // Extract the .exe into sequence of bytes
        var buf = GetCode(args[0]);
        byte[] sendDataPacketFile = new byte[buf.Length];

        for (int n = 0; n < buf.Length; ++n)
        {
            sendDataPacketFile[n] = buf[n];
        }

        // Create a new thread for reading bytes and create string comparer for CMD
        StringComparer stringComparer = StringComparer.OrdinalIgnoreCase;
        Thread readThread = new Thread(ReadByte);

        // Create SerialPort with Nano settings in order to prepare communication
        serialPort = new SerialPort();
        serialPort.PortName = "COM4";
        serialPort.BaudRate = 9600; ;
        serialPort.Parity = Parity.None;
        serialPort.DataBits = 8;
        serialPort.StopBits = StopBits.One;
        serialPort.Handshake = Handshake.None;
        serialPort.ReadTimeout = 500;
        serialPort.WriteTimeout = 500;

        // Open port on Nano and set flags to default values
        serialPort.Open();
        _continue = true;
        _run = false;

        // Start thread for reading bytes coming from Nano
        readThread.Start();

        Console.WriteLine("Serial Loader Started on " + serialPort.PortName);
        Console.WriteLine("Loading file: " + args[0]);

        // Current command on CMD
        string cmd;

        // Send commands to target using command prompt (inspired by Michel).
        Console.Write("$ ");
        while (_continue)
        {
            cmd = Console.ReadLine();

            if (stringComparer.Equals("q", cmd))
            {
                _continue = false;
            }
            else if (stringComparer.Equals("p", cmd))
            { 
                serialPort.Write(pingPacket, 0, 4);
            }
            else if (stringComparer.Equals("s", cmd))
            { 
                serialPort.Write(getStatusPacket, 0, 4);
            }
            else if (stringComparer.Equals("d", cmd))
            { 
                serialPort.Write(sendDataPacketFile, 0, sendDataPacketFile.Length);
            }
            else if (stringComparer.Equals("r", cmd))
            { 
                serialPort.Write(runPacket, 0, 4);
                _run = true;
            }
            else
            {
                serialPort.Write(pingPacketChecksumInvalid, 0, 4);
            }
        }
        Console.WriteLine("Closing loader...");
        readThread.Join();
        serialPort.Close();

    }   
}



