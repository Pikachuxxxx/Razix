﻿//Copyright © 2014 Sony Computer Entertainment America LLC. See License.txt.

using System;
using System.Drawing;
using System.Drawing.Drawing2D;


using Sce.Atf.VectorMath;

namespace Sce.Atf.Controls.Adaptable
{
    /// <summary>
    /// Interface for adapters that define a transform for the adapted control
    /// consisting of a scale, followed by a translation</summary>
    public interface ITransformAdapter : IControlAdapter
    {
        /// <summary>
        /// Gets the current transformation matrix, from world to Window client coordinates</summary>
        Matrix Transform
        {
            get;
        }

        /// <summary>
        /// Gets or sets the translation</summary>
        PointF Translation
        {
            get;
            set;
        }

        /// <summary>
        /// Gets or sets the minimum values for x and y translation</summary>
        PointF MinTranslation
        {
            get;
            set;
        }

        /// <summary>
        /// Gets or sets the maximum values for x and y translation</summary>
        PointF MaxTranslation
        {
            get;
            set;
        }

        /// <summary>
        /// Gets or sets the scale</summary>
        PointF Scale
        {
            get;
            set;
        }

        /// <summary>
        /// Gets or sets the minimum values for x and y scale</summary>
        PointF MinScale
        {
            get;
            set;
        }

        /// <summary>
        /// Gets or sets the maximum values for x and y scale</summary>
        PointF MaxScale
        {
            get;
            set;
        }

        /// <summary>
        /// Gets a value indicating if the transform scale is constrained to be uniform
        /// (same in x and y direction)</summary>
        bool UniformScale
        {
            get;
        }

        /// <summary>
        /// Sets the transform's scale and translation</summary>
        /// <param name="xScale">X scale</param>
        /// <param name="yScale">Y scale</param>
        /// <param name="xTranslation">X translation</param>
        /// <param name="yTranslation">Y translation</param>
        void SetTransform(float xScale, float yScale, float xTranslation, float yTranslation);

        /// <summary>
        /// Event that is raised after the transform changes</summary>
        event EventHandler TransformChanged;
    }

    /// <summary>
    /// Useful static/extension methods on ITransformAdapter</summary>
    public static class TransformAdapters
    {
        /// <summary>
        /// Sets the transform to the given matrix; rotation and skew are
        /// ignored</summary>
        /// <param name="transformAdapter">Adapter managing transform</param>
        /// <param name="transform">Transformation matrix, from world to Window client coordinates</param>
        public static void SetTransform(this ITransformAdapter transformAdapter, Matrix transform)
        {
            float[] m = transform.Elements;
            transformAdapter.SetTransform(m[0], m[3], m[4], m[5]);
        }

        /// <summary>
        /// Transforms client point to world coordinates</summary>
        /// <param name="adapter">Adapter managing transform</param>
        /// <param name="x">Point, in client coordinates</param>
        /// <returns>Point, in world coordinates</returns>
        public static Point ClientToTransform(this ITransformAdapter adapter, Point x)
        {
            return GdiUtil.InverseTransform(adapter.Transform, x);
        }

        /// <summary>
        /// Transforms client point to world coordinates</summary>
        /// <param name="adapter">Adapter managing transform</param>
        /// <param name="x">Point, in client coordinates</param>
        /// <returns>Point, in world coordinates</returns>
        public static PointF ClientToTransform(this ITransformAdapter adapter, PointF x)
        {
            return GdiUtil.InverseTransform(adapter.Transform, x);
        }

        /// <summary>
        /// Transforms client rectangle to world coordinates</summary>
        /// <param name="adapter">Adapter managing transform</param>
        /// <param name="x">Rectangle, in client coordinates</param>
        /// <returns>Rectangle, in world coordinates</returns>
        public static Rectangle ClientToTransform(this ITransformAdapter adapter, Rectangle x)
        {
            return GdiUtil.InverseTransform(adapter.Transform, x);
        }

        /// <summary>
        /// Transforms client rectangle to world coordinates</summary>
        /// <param name="adapter">Adapter managing transform</param>
        /// <param name="x">Rectangle, in client coordinates</param>
        /// <returns>Rectangle, in world coordinates</returns>
        public static RectangleF ClientToTransform(this ITransformAdapter adapter, RectangleF x)
        {
            return GdiUtil.InverseTransform(adapter.Transform, x);
        }

        /// <summary>
        /// Transforms point to client coordinates</summary>
        /// <param name="adapter">Adapter managing transform</param>
        /// <param name="x">Point, in world coordinates</param>
        /// <returns>Point, in client coordinates</returns>
        public static Point TransformToClient(this ITransformAdapter adapter, Point x)
        {
            return GdiUtil.Transform(adapter.Transform, x);
        }

        /// <summary>
        /// Transforms point to client coordinates</summary>
        /// <param name="adapter">Adapter managing transform</param>
        /// <param name="x">Point, in world coordinates</param>
        /// <returns>Point, in client coordinates</returns>
        public static PointF TransformToClient(this ITransformAdapter adapter, PointF x)
        {
            return GdiUtil.Transform(adapter.Transform, x);
        }

        /// <summary>
        /// Transforms rectangle to client coordinates</summary>
        /// <param name="adapter">Adapter managing transform</param>
        /// <param name="x">Rectangle, in world coordinates</param>
        /// <returns>Rectangle, in client coordinates</returns>
        public static Rectangle TransformToClient(this ITransformAdapter adapter, Rectangle x)
        {
            return GdiUtil.Transform(adapter.Transform, x);
        }

        /// <summary>
        /// Transforms rectangle to client coordinates</summary>
        /// <param name="adapter">Adapter managing transform</param>
        /// <param name="x">Rectangle, in world coordinates</param>
        /// <returns>Rectangle, in client coordinates</returns>
        public static RectangleF TransformToClient(this ITransformAdapter adapter, RectangleF x)
        {
            return GdiUtil.Transform(adapter.Transform, x);
        }

        /// <summary>
        /// Constrains translation to the adapter's MinTranslation and MaxTranslation
        /// constraints</summary>
        /// <param name="adapter">Transform adapter</param>
        /// <param name="translation">Desired translation</param>
        /// <returns>Translation, constrained to the adapter's limits</returns>
        public static PointF ConstrainTranslation(this ITransformAdapter adapter, PointF translation)
        {
            PointF minTranslation = adapter.MinTranslation;
            PointF maxTranslation = adapter.MaxTranslation;
            return new PointF(
                Math.Max(minTranslation.X, Math.Min(maxTranslation.X, translation.X)),
                Math.Max(minTranslation.Y, Math.Min(maxTranslation.Y, translation.Y)));
        }

        /// <summary>
        /// Constrains scale to the adapter's MinScale, MaxScale, and UniformScale
        /// constraints</summary>
        /// <param name="adapter">Transform adapter</param>
        /// <param name="scale">Desired scale</param>
        /// <returns>Scale, constrained to the adapter's limits</returns>
        /// <remarks>If UniformScale is true, the maximum x or y scale is used.</remarks>
        public static PointF ConstrainScale(this ITransformAdapter adapter, PointF scale)
        {
            PointF minScale = adapter.MinScale;
            PointF maxScale = adapter.MaxScale;
            if (adapter.UniformScale)
            {
                scale.X = scale.Y = Math.Max(scale.X, scale.Y);
            }

            return new PointF(
                Math.Max(minScale.X, Math.Min(maxScale.X, scale.X)),
                Math.Max(minScale.Y, Math.Min(maxScale.Y, scale.Y)));
        }

        
       

        /// <summary>
        /// Pans the view the minimum necessary so that the given rectangle is visible</summary>
        /// <param name="adapter">Adapter managing transform</param>
        /// <param name="bounds">Rectangle to make visible, in Windows client coordinates</param>
        public static void PanToRect(this ITransformAdapter adapter, RectangleF bounds)
        {
            RectangleF clientRect = adapter.AdaptedControl.ClientRectangle;

            if (clientRect.IsEmpty)
                return;

            float dx;
            if (bounds.Right > clientRect.Right)
                dx = bounds.Right - clientRect.Right;
            else if (bounds.Left < clientRect.Left)
                dx = bounds.Left - clientRect.Left;
            else
                dx = 0;

            float dy;
            if (bounds.Bottom > clientRect.Bottom)
                dy = bounds.Bottom - clientRect.Bottom;
            else if (bounds.Top < clientRect.Top)
                dy = bounds.Top - clientRect.Top;
            else
                dy = 0;

            if (dx == 0 && dy == 0)
                return; // already visible

            PointF currentTranslation = adapter.Translation;
            adapter.Translation = new PointF(
                currentTranslation.X - dx,
                currentTranslation.Y - dy);
        }
    }
}
