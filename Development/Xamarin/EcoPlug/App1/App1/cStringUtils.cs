using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

class cStringUtils
{
    public static byte[] toByteArray(string _str)
    {
        byte[] ba = new byte[_str.Length];
        for (int r = 0; r < _str.Length; r++)
        {
            byte b = (byte)_str[r];
            ba[r] = b;
        }
        return ba;
    }

    public static String toString(byte[] _buffer)
    {
        String result = "";
        for (int r = 0; r < _buffer.Length; r++)
        {
            result += _buffer[r];
        }
        return result;
    }

}

