using Voxen;

namespace Demo
{
    public class Camera : Entity
    {
        public float speed = 1.0f;
        public Entity target;

        private Vector3 Lerp(Vector3 current, Vector3 target, float speed, float ts)
        {
            // Linear interpolation for each component (X, Y, Z)
            float newX = current.X + (target.X - current.X) * speed * ts;
            float newY = current.Y + (target.Y - current.Y) * speed * ts;
            float newZ = current.Z + (target.Z - current.Z) * speed * ts;

            // Return the new interpolated Vector3
            return new Vector3(newX, newY, newZ);
        }

        void OnCreate()
        {
            target = FindEntityByName("Player");
        }

        void OnUpdate(float ts)
        {
            if (target == null) return;

            Vector3 targetPosition = new Vector3 (target.Translation.X, target.Translation.Y, Translation.Z);

            Translation = Lerp(Translation, targetPosition, speed, ts);
        }

    }
}
