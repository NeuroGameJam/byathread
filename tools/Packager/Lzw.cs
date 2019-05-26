namespace Packager
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;

    public class Lzw
    {
        private const int MAX_BITS = 14; //maimxum bits allowed to read
        private const int HASH_BIT = MAX_BITS - 8; //hash bit to use with the hasing algorithm to find correct index
        private const int MAX_VALUE = (1 << MAX_BITS) - 1; //max value allowed based on max bits
        private const int MAX_CODE = MAX_VALUE - 1; //max code possible
        private const int TABLE_SIZE = 18041; //must be bigger than the maximum allowed by maxbits and prime

        private int[] _iaCodeTable = new int[TABLE_SIZE]; //code table
        private int[] _iaPrefixTable = new int[TABLE_SIZE]; //prefix table
        private int[] _iaCharTable = new int[TABLE_SIZE]; //character table

        private ulong _iBitBuffer; //bit buffer to temporarily store bytes read from the files
        private int _iBitCounter; //counter for knowing how many bits are in the bit buffer

        private List<byte> result;
        private int readPos;

        private void Initialize() //used to blank  out bit buffer incase this class is called to comprss and decompress from the same instance
        {
            _iBitBuffer = 0;
            _iBitCounter = 0;
            result = new List<byte>();
            readPos = 0;
        }

        public List<Byte> Compress(List<byte> source)
        {
            try
            {
                Initialize();

                int iNextCode = 256;
                int iString = 0, iIndex = 0;

                for (int i = 0; i < TABLE_SIZE; i++) //blank out table
                    _iaCodeTable[i] = -1;

                iString = source[0]; //get first code, will be 0-255 ascii char

                foreach (int bNext in source.Skip(1))
                //while ((iChar = reader.ReadByte()) != -1) //read until we reach end of file
                {
                    iIndex = FindMatch(iString, bNext); //get correct index for prefix+char

                    if (_iaCodeTable[iIndex] != -1) //set string if we have something at that index
                        iString = _iaCodeTable[iIndex];
                    else //insert new entry
                    {
                        if (iNextCode <= MAX_CODE) //otherwise we insert into the tables
                        {
                            _iaCodeTable[iIndex] = iNextCode++; //insert and increment next code to use
                            _iaPrefixTable[iIndex] = iString;
                            _iaCharTable[iIndex] = bNext;
                        }

                        WriteCode(iString); //output the data in the string
                        iString = bNext;
                    }
                }

                WriteCode(iString); //output last code
                WriteCode(MAX_VALUE); //output end of buffer
                WriteCode(0); //flush
            }
            catch
            {
                throw new Exception();
            }
            finally
            {
            }

            return result;
        }

        //hasing function, tries to find index of prefix+char, if not found returns -1 to signify space available
        private int FindMatch(int pPrefix, int pChar)
        {
            int index = 0, offset = 0;

            index = (pChar << HASH_BIT) ^ pPrefix;

            offset = (index == 0) ? 1 : TABLE_SIZE - index;

            while (true)
            {
                if (_iaCodeTable[index] == -1)
                    return index;

                if (_iaPrefixTable[index] == pPrefix && _iaCharTable[index] == pChar)
                    return index;

                index -= offset;
                if (index < 0)
                    index += TABLE_SIZE;
            }
        }

        private void WriteCode(int pCode)
        {
            _iBitBuffer |= (ulong)pCode << (32 - MAX_BITS - _iBitCounter); //make space and insert new code in buffer
            _iBitCounter += MAX_BITS; //increment bit counter

            while (_iBitCounter >= 8) //write all the bytes we can
            {
                result.Add((byte)((_iBitBuffer >> 24) & 255)); //write byte from bit buffer                
                _iBitBuffer <<= 8; //remove written byte from buffer
                _iBitCounter -= 8; //decrement counter
            }
        }

        public List<byte> Decompress(List<byte> source)
        {
            try
            {
                Initialize();

                int iNextCode = 256;
                int iNewCode, iOldCode;
                byte bChar;
                int iCurrentCode, iCounter;
                byte[] baDecodeStack = new byte[TABLE_SIZE];

                iOldCode = ReadCode(source);
                bChar = (byte)iOldCode;

                result.Add((byte)iOldCode); //write first byte since it is plain ascii

                iNewCode = ReadCode(source);

                while (iNewCode != MAX_VALUE) //read file all file
                {
                    if (iNewCode >= iNextCode)
                    { //fix for prefix+chr+prefix+char+prefx special case
                        baDecodeStack[0] = bChar;
                        iCounter = 1;
                        iCurrentCode = iOldCode;
                    }
                    else
                    {
                        iCounter = 0;
                        iCurrentCode = iNewCode;
                    }

                    while (iCurrentCode > 255) //decode string by cycling back through the prefixes
                    {
                        baDecodeStack[iCounter] = (byte)_iaCharTable[iCurrentCode];
                        ++iCounter;
                        if (iCounter >= MAX_CODE)
                            throw new Exception("oh crap");
                        iCurrentCode = _iaPrefixTable[iCurrentCode];
                    }

                    baDecodeStack[iCounter] = (byte)iCurrentCode;
                    bChar = baDecodeStack[iCounter]; //set last char used

                    while (iCounter >= 0) //write out decodestack
                    {
                        result.Add(baDecodeStack[iCounter]);
                        --iCounter;
                    }

                    if (iNextCode <= MAX_CODE) //insert into tables
                    {
                        _iaPrefixTable[iNextCode] = iOldCode;
                        _iaCharTable[iNextCode] = bChar;
                        ++iNextCode;
                    }

                    iOldCode = iNewCode;

                    iNewCode = ReadCode(source);
                }
            }
            catch (Exception)
            {
                throw new Exception();
            }

            return result;
        }

        private int ReadCode(List<byte> source)
        {
            uint iReturnVal;

            while (_iBitCounter <= 24) //fill up buffer
            {
                byte val = source[readPos];
                readPos++;
                _iBitBuffer |= (ulong)val << (24 - _iBitCounter); //insert byte into buffer
                _iBitCounter += 8; //increment counter
            }

            iReturnVal = (uint)_iBitBuffer >> (32 - MAX_BITS); //get last byte from buffer so we can return it
            _iBitBuffer <<= MAX_BITS; //remove it from buffer
            _iBitCounter -= MAX_BITS; //decrement bit counter

            int temp = (int)iReturnVal;
            return temp;
        }
    }
}
