﻿//Copyright © 2014 Sony Computer Entertainment America LLC. See License.txt.

using System;
using System.Drawing;
using System.Globalization;
using System.Windows.Forms;

namespace Sce.Atf.Controls
{

    /// <summary>
    /// Control for editing a bounded int value</summary>
    public class IntInputControl : UserControl
    {
        /// <summary>
        /// Constructor</summary>
        public IntInputControl()
            : this(0, 0, 100)
        {
        }

        /// <summary>
        /// Constructor with initial and bounding values</summary>
        /// <param name="value">Initial value</param>
        /// <param name="min">Minimum value</param>
        /// <param name="max">Maximum value</param>
        public IntInputControl(int value, int min, int max)
        {
            if (min >= max)
                throw new ArgumentException("min must be less than max");
            DoubleBuffered = true;
            m_min = min;
            m_max = max;
            m_value = MathUtil.Clamp(value, m_min, m_max);
            m_lastChange = m_value;
            m_lastEdit = m_value;
            
            m_textBox = new NumericTextBox(typeof(int));
            m_textBox.BorderStyle = BorderStyle.None;
            m_textBox.Name = "m_textBox";

            m_spinner = new CompactSpinner();
            m_spinner.GotFocus += (sender, e) => m_textBox.Focus();

            SuspendLayout();            
            UpdateTextBox();                        
            Controls.Add(m_textBox);            
            Controls.Add(m_spinner);                        
            ResumeLayout(false);
            PerformLayout();
            
            m_textBox.ValueEdited += (sender, e) =>
                {
                    int val = (int)m_textBox.Value;
                    SetValue(val, false);
                    EndEdit(true);
                };

            m_spinner.Changed += (sender, e) =>
                {                   
                    int newValue = Value + e.Value;
                    SetValue(newValue, false);
                };

            m_textBox.SizeChanged += (sender, e) => this.Height = m_textBox.Height + 3;                
            SizeChanged += (sender, e) =>
                {
                    m_spinner.Bounds = new Rectangle(0, 0, Height, Height);
                    m_textBox.Bounds = new Rectangle(m_spinner.Width, 0, Width - m_spinner.Width, m_textBox.Height);
                };
        }

        /// <summary>
        /// Gets and sets the current value of the control</summary>
        public int Value
        {
            get { return m_value; }
            set
            {
                SetValue(value, false);
            }
        }

        /// <summary>
        /// Gets the last changed value</summary>
        public int LastChange
        {
            get { return m_lastChange; }
        }

        /// <summary>
        /// Gets the last edited value</summary>
        public int LastEdit
        {
            get { return m_lastEdit; }
        }

        /// <summary>
        /// Gets or sets the minimum value</summary>
        public int Min
        {
            get { return m_min; }
            set
            {
                SetMinMax(value, m_max);
                this.Invalidate();
            }
        }

        /// <summary>
        /// Gets or sets the maximum value</summary>
        public int Max
        {
            get { return m_max; }
            set
            {
                SetMinMax(m_min, value);
            }
        }

        /// <summary>
        /// Sets min and max
        /// </summary>
        /// <param name="min">min value</param>
        /// <param name="max">max value</param>
        public void SetMinMax(int min, int max)
        {
            if (min >= max)
                throw new ArgumentOutOfRangeException("min must be less than max");
            if (m_min != min || m_max != max)
            {
                m_min = min;
                m_max = max;
                Value = MathUtil.Clamp(m_value, m_min, m_max);
            }
        }

        /// <summary>
        /// Gets and sets whether to draw a border around the control</summary>
        public bool DrawBorder
        {
            get { return m_drawBorder; }
            set { m_drawBorder = value; }
        }

        /// <summary>
        /// Event that is raised after value is changed</summary>
        public event EventHandler ValueChanged;

        /// <summary>
        /// Event that is raised after the value is edited</summary>
        public event EventHandler ValueEdited;

        /// <summary>
        /// Raises the <see cref="E:Sce.Atf.Controls.IntInputControl.ValueChanged"/> event</summary>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data</param>
        protected virtual void OnValueChanged(EventArgs e)
        {
            ValueChanged.Raise(this, e);
        }

        /// <summary>
        /// Raises the <see cref="E:Sce.Atf.Controls.IntInputControl.ValueEdited"/> event</summary>
        /// <param name="e">The <see cref="System.EventArgs"/> instance containing the event data</param>
        protected virtual void OnValueEdited(EventArgs e)
        {
            ValueEdited.Raise(this, e);
        }

        /// <summary>
        /// Performs custom actions on Paint events. Draws grid and curve.</summary>        
        /// <param name="e">Paint event args</param>
        protected override void OnPaint(PaintEventArgs e)
        {
            float r = (float)(m_value - m_min) / (float)(m_max - m_min);
            int w = (int)(r * m_textBox.Width);
            Rectangle rec
                = new Rectangle(m_textBox.Location.X, m_textBox.Height, w, 3);
            e.Graphics.FillRectangle(Brushes.LightBlue, rec);
            if (m_drawBorder)
                ControlPaint.DrawBorder3D(e.Graphics, ClientRectangle, Border3DStyle.Flat);
        }
       
        private void EndEdit(bool forceNewValue)
        {
            if (forceNewValue ||
                m_value != m_lastEdit)
            {
                OnValueEdited(EventArgs.Empty);
                m_lastEdit = m_value;
            }
        }

        private void SetValue(int value, bool forceNewValue)
        {
            value = Sce.Atf.MathUtil.Clamp(value, m_min, m_max);        
            if (forceNewValue ||
                value != m_value)
            {
                m_value = value;
                OnValueChanged(EventArgs.Empty);
                m_lastChange = value;
            }

            // Update the user interface to make sure the displayed text is in sync.
            //  If these two methods are in the above 'if', then typing in the same
            //  out-of-range value twice in a row persists, indicating that m_textBox.Text
            //  is out of sync with m_value.
            UpdateTextBox();
            this.Invalidate();
        }

       

        private void UpdateTextBox()
        {

            m_textBox.Value = m_value;
            //m_textBox.Text = m_value.ToString(CultureInfo.CurrentCulture);
        }

       
        private int m_value;
        private int m_lastChange;
        private int m_lastEdit;
        private int m_min;
        private int m_max;

        private bool m_drawBorder = true;

        private CompactSpinner m_spinner;
        private NumericTextBox m_textBox;
        
    }


}
