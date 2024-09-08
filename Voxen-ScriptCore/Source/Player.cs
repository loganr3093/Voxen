using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Voxen;
using static System.Runtime.CompilerServices.RuntimeHelpers;

namespace Demo
{
    public class Player : Entity
    {
        private TransformComponent m_Transform;

        void OnCreate()
        {
            Console.WriteLine($"Player.OnCreate - {ID}");

            m_Transform = GetComponent<TransformComponent>();

        }

        void OnUpdate(float ts)
        {
            // Console.WriteLine($"Player.OnUpdate: {ts}");

            float speed = 1;
            Vector3 velocity = Vector3.Zero;

            if (Input.IsKeyDown(KeyCode.W))
            {
                velocity.Y = 1.0f;
            }
            else if (Input.IsKeyDown(KeyCode.S))
            {
                velocity.Y = -1.0f;
            }

            if (Input.IsKeyDown(KeyCode.D))
            {
                velocity.X = 1.0f;
            }
            else if (Input.IsKeyDown(KeyCode.A))
            {
                velocity.X = -1.0f;
            }

            velocity *= speed;

            Vector3 translation = m_Transform.Translation;

            translation += velocity * ts;

            m_Transform.Translation = translation;
        }
    }
}
