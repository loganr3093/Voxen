using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Voxen;

namespace Demo
{
    public class Player : Entity
    {
        void  OnCreate()
        {
            Console.WriteLine("Player.OnCreate");

            
        }

        void OnUpdate(float ts)
        {
            Console.WriteLine($"Player.OnUpdate: {ts}");
        }
    }
}
