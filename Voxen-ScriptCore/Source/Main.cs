using System;

namespace Voxen
{
    public class Main
    {
        public float FloatVar { get; set; }

        public Main()
        {
            Console.WriteLine("Main Constructor");
        }

        public void PrintMessage()
        {
            Console.WriteLine("Hello World");
        }

        public void PrintInt(int num)
        {
            Console.WriteLine($"{num}");
        }

        public void PrintCustomMessage(string message)
        {
            Console.WriteLine($"{message}");
        }
    }
}