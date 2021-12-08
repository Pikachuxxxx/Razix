//Copyright � 2014 Sony Computer Entertainment America LLC. See License.txt.

using System;
using System.Drawing;
using System.Globalization;
using System.Windows.Forms;
using System.Windows.Forms.VisualStyles;

namespace Sce.Atf.Controls
{
    /// <summary>
    /// Class to edit a float value using a Textbox, optionally with a slider tool.</summary>
    public class FloatDataEditor : DataEditor
    {

        /// <summary>
        /// Initializes a new instance of the <see cref="FloatDataEditor"/> class.</summary>
        /// <param name="theme">The visual theme to use</param>
        public FloatDataEditor(DataEditorTheme theme)
            : base(theme)
        {
            SliderWidth = theme.DefaultSliderWidth;
            Epsilon = 0.000001f;
        }

        /// <summary>
        /// Get or set absolute margin of difference for the two float values to be considered equal.</summary>
        public float Epsilon { get; set; }

        /// <summary>
        /// The float value to display and edit.</summary>
        public float Value;

        /// <summary>
        /// The minimum value allowed.</summary>
        public float Min;
        /// <summary>
        /// The maximum value allowed.</summary>
        public float Max;

        /// <summary>
        /// Whether to show the slider.</summary>
        public bool ShowSlider;

        private float m_startValue;


        /// <summary>
        /// Layout: Measures the desired layout size of the data value and any child UI elements.</summary>
        /// <param name="g">Graphics that can be used for measuring strings</param>
        /// <param name="availableSize">The available size that this editor can give to child UI elements</param>
        /// <returns>
        /// The size that this editor determines it needs during layout, based on its calculations of child element sizes</returns>
        public override SizeF Measure(Graphics g,  SizeF availableSize)
        {

            SizeF size = g.MeasureString(Value.ToString("F"), Theme.Font);
            size.Width += Theme.Padding.Left;
            if (ShowSlider)
            {
                size.Width += SliderWidth + Theme.Padding.Left;
            }
            return size;
        }

        /// <summary>
        /// Implement the display of the value's representation.</summary>
        /// <param name="g">The Graphics object</param>
        /// <param name="area">Rectangle delimiting area to paint</param>
        public override void PaintValue(Graphics g,  Rectangle area)
        {
            int textOffset = 0;
            int left = area.Left + Theme.Padding.Left;
            if (ShowSlider)
            {
                // draw track
                float slideY = area.Y + area.Height / 2;
                g.DrawLine(Theme.SliderTrackPen, left, slideY, left + SliderWidth, slideY);

                // draw thumb
                float t = (Value - Min) / (Max - Min);
                float thumbX = left + t * SliderWidth;
                var thumbRectangle = new Rectangle((int)thumbX - 8, (int)area.Top, 18, 18);
                if (TrackBarRenderer.IsSupported)
                    TrackBarRenderer.DrawBottomPointingThumb(g, thumbRectangle, TrackBarThumbState.Normal);
                else // visual styles may be disabled by the user in the operating system, roll our own
                {
                    thumbRectangle = new Rectangle((int) thumbX - 4, (int) area.Top, 8, 16);
                    DrawThumb(g, thumbRectangle, TrackBarThumbState.Normal);
                }

                textOffset = SliderWidth + Theme.Padding.Left;
            }

            string valueString = ToString();
            g.DrawString(valueString, Theme.Font, Theme.TextBrush, left + textOffset, area.Top); 
        }

        private void DrawThumb(Graphics g, Rectangle bounds, TrackBarThumbState state)
        {
            s_thumbPoints[0] = bounds.Location;
            s_thumbPoints[1] = new Point(bounds.Right, bounds.Top);
            s_thumbPoints[2] = new Point(bounds.Right, bounds.Top + bounds.Height * 3 / 4);
            s_thumbPoints[3] = new Point((bounds.Left + bounds.Right) / 2, bounds.Top + bounds.Height);
            s_thumbPoints[4] = new Point(bounds.Left, bounds.Top + bounds.Height * 3 / 4);

            g.DrawPolygon(Theme.SliderTrackPen, s_thumbPoints);
        }

        private static Point[] s_thumbPoints = new Point[5];

        /// <summary>
        /// Determines the editing mode from input position.</summary>
        /// <param name="p">Input position point</param>
        public override void SetEditingMode(Point p)
        {
            int x = p.X - Bounds.Left;

            if (ShowSlider && x >= Theme.Padding.Left && x <= Theme.Padding.Left + SliderWidth)
                EditingMode = EditMode.BySlider;
            else
            {
                EditingMode = EditMode.ByTextBox;
            }
        }

        /// <summary>
        /// Begins an edit operation.</summary>
        public override void BeginDataEdit()
        {
            m_startValue = Value;

            if (EditingMode == EditMode.ByTextBox)
            {
                int textBoxOffset = ShowSlider ? SliderWidth + Theme.Padding.Left : 0;
                TextBox.Text = Value.ToString("F");
                TextBox.Bounds = new Rectangle(Bounds.Left + textBoxOffset, Bounds.Top, Bounds.Width - textBoxOffset, Bounds.Height);
                TextBox.SelectAll();
                TextBox.Show();
                TextBox.Focus();
            }
        }


        /// <summary>
        /// Ends an edit operation.</summary>
        /// <returns>
        /// 'true' if the change should be committed and 'false' if the change should be discarded</returns>
        public override bool EndDataEdit()
        {
            if (EditingMode == EditMode.ByTextBox)
            {
                Parse(TextBox.Text);
            }
            return (!(m_startValue == Value || (Math.Abs(Value - m_startValue) < Epsilon)));
        }


        /// <summary>
        /// Performs custom actions when moving the mouse pointer over this editor.</summary>
        /// <param name="e">The <see cref="MouseEventArgs" /> instance containing the event data</param>
        public override void OnMouseMove(MouseEventArgs e)
        {
            if (ShowSlider && EditingMode == EditMode.BySlider)
                Value = GetSliderFloatValue(e.X);
        }

        /// <summary>
        /// Performs custom actions when the user clicks the editor with either mouse button.</summary>
        /// <param name="e">The <see cref="MouseEventArgs" /> instance containing the event data</param>
        public override void OnMouseDown(MouseEventArgs e)
        {
            if (ShowSlider && EditingMode == EditMode.BySlider)
                Value = GetSliderFloatValue(e.X);
        }

        private float GetSliderFloatValue(int x)
        {
            if (!ShowSlider)
                throw new InvalidOperationException("ShowSlider must be true");
            float t = ((float)(x - Bounds.Left - Theme.Padding.Left)) / SliderWidth;
            float newValue = Min + t * (Max - Min);
            //System.Diagnostics.Trace.TraceInformation("Slider value {0}", newValue); 
            return MathUtil.Clamp(newValue, Min, Max);           
        }

        /// <summary>
        /// Determines whether the data editor wants to track mouse movement.</summary>
        /// <returns>
        /// Whether data editor wants to track mouse movement</returns>
        public override bool WantsMouseTracking()
        {
            return EditingMode == EditMode.BySlider;
        }


        /// <summary>
        /// Returns a <see cref="System.String" /> that represents this instance.</summary>
        /// <returns>
        /// A <see cref="System.String" /> that represents this instance.</returns>
        public override string ToString()
        {
            return Value.ToString("F");
        }

        /// <summary>
        /// Parses the specified string representation and sets the data value.</summary>
        /// <param name="s">String to parse</param>
        public override void Parse(string s)
        {
            float singleResult;
            if (Single.TryParse(s, NumberStyles.Float, CultureInfo.CurrentCulture, out singleResult))
            {
                if (Min == 0F && Max == 0F) // range not set
                    Value = singleResult;
                else
                    Value = MathUtil.Clamp(singleResult, Min, Max);               
            }
        }

        /// <summary>
        /// Gets or sets the width of the slider. Is only useful if ShowSlider is true.</summary>
        /// <value>
        /// The width of the slider.</value>
        public int SliderWidth
        {
            get { return m_sliderWidth; }
            set { m_sliderWidth = value; }
        }

        private int m_sliderWidth;
    }
}