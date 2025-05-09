using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Voxen;

namespace Demo
{
    public class Bird : Entity
    {
        private TransformComponent m_Transform;

        public float gravity = -20.0f;
        public float jumpForce = 6.0f;
        public float maxRotation = 45.0f;
        public float minRotation = -75.0f;
        public float rotationSpeed = 5.0f;

        private float m_VerticalVelocity = 0.0f;

        private bool keyDown = false;

        void OnCreate()
        {
            Console.WriteLine($"Player.OnCreate - {ID}");

            m_Transform = GetComponent<TransformComponent>();

        }

        void OnUpdate(float ts)
        {
            if (Input.IsKeyDown(KeyCode.Space))
            {
                if (!keyDown)
                {
                    m_VerticalVelocity = jumpForce;
                    keyDown = true;
                }
            }
            else
            {
                keyDown = false;
            }

            m_VerticalVelocity += gravity * ts;

            var pos = m_Transform.Translation;
            pos.Y += m_VerticalVelocity * ts;
            m_Transform.Translation = pos;
        }
    }
}
