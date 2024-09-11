namespace Voxen
{
    public struct Vector4
    {
        public float X, Y, Z, W;

        public static Vector4 Zero => new Vector4(0.0f, 0.0f, 0.0f, 0.0f);
        public static Vector4 One => new Vector4(1.0f, 1.0f, 1.0f, 1.0f);

        public Vector4(float scalar)
        {
            X = scalar;
            Y = scalar;
            Z = scalar;
            W = scalar;
        }

        public Vector4(float x, float y, float z, float w)
        {
            X = x;
            Y = y;
            Z = z;
            W = w;
        }

        public float Magnitude => (float)System.Math.Sqrt(X * X + Y * Y + Z * Z + W * W);
        public Vector4 Normalized => this * (1.0f / Magnitude);

        public static Vector4 Lerp(Vector4 a, Vector4 b, float t)
        {
            return a + (b - a) * t;
        }

        public static float Dot(Vector4 a, Vector4 b)
        {
            return a.X * b.X + a.Y * b.Y + a.Z * b.Z + a.W * b.W;
        }

        public static Vector4 operator +(Vector4 a, Vector4 b)
        {
            return new Vector4(a.X + b.X, a.Y + b.Y, a.Z + b.Z, a.W + b.W);
        }

        public static Vector4 operator -(Vector4 a, Vector4 b)
        {
            return new Vector4(a.X - b.X, a.Y - b.Y, a.Z - b.Z, a.W - b.W);
        }

        public static Vector4 operator *(Vector4 vector, float scalar)
        {
            return new Vector4(vector.X * scalar, vector.Y * scalar, vector.Z * scalar, vector.W * scalar);
        }

        public override string ToString()
        {
            return $"({X}, {Y}, {Z}, {W})";
        }
    }
}
