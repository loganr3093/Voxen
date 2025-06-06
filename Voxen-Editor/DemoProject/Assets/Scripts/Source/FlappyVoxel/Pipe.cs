using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Voxen;

namespace Demo
{
    public class Pipe : Entity
    {
        private TransformComponent m_Transform;

        public float pipeID = 0.0f;

        private float speed = 75.0f;

        private float startDelay = 2.0f;
        private bool started = false;

        private float baseDistance = 250.0f;
        private float gapDistance = 100.0f;

        void OnCreate()
        {
            Console.WriteLine($"Player.OnCreate - {ID}");

            m_Transform = GetComponent<TransformComponent>();

            Vector3 translation = m_Transform.Translation;
            translation.X = baseDistance + (pipeID * gapDistance);
            m_Transform.Translation = translation;
        }

        void OnUpdate(float ts)
        {
            if (!started)
            {
                startDelay -= ts;
                if (startDelay <= 0.0f)
                {
                    started = true;
                }
                return;
            }

            Vector3 velocity = Vector3.Left;

            velocity *= speed * ts;

            Vector3 translation = m_Transform.Translation;

            translation += velocity;

            if (translation.X <= -baseDistance + (pipeID * gapDistance))
            {
                translation.X = baseDistance + (pipeID * gapDistance);
                m_Transform.Translation = translation;
            }
            else
            {
                m_Transform.Translation = translation;
            }
        }
    }
}
