using System;
using Voxen;

namespace Demo
{
    public class DemoClass : Entity
    {
        void OnCreate()
        {
            Console.WriteLine($"DemoClass.OnCreate - {ID}");
        }

        void OnUpdate(float ts)
        {
        }
    }
}