﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;

namespace GLGCS
{
	public class Parameter : INotifyPropertyChanged
	{
		public event PropertyChangedEventHandler PropertyChanged;

		int id;
		string name;
		double value;
		int type;

		public int ID
		{
			get => id;
			set
			{
				id = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("ID"));
			}
		}
		public double Value
		{
			get => value;
			set
			{
				this.value = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("Value"));
			}
		}
		public string Name
		{
			get => name;
			set
			{
				name = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("Value"));
			}
		}
		public int Type
		{
			get => type;
			set
			{
				type = value;
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("Type"));
			}
		}

		public string GetCmd()
		{
			string sign = value < 0 ? "-" : "+";
			double vt = value;
			for (int i = 0; i < type; i++)
				vt *= 10;
			int vi = (int)Math.Round(Math.Abs(vt));
			if (vi >= 100000000)
				vi = 99999999;
			return string.Format("*set{0:D2}{1}{2:D8}/", id, sign, vi);
		}

		public void SetBinaryValue(double v)
		{
			int vi = (int)Math.Round(Math.Abs(v));
			int one = 1, res = 0;
			while (vi > 0)
			{
				if (vi % 2 == 1)
					res += one;
				one *= 10;
				vi >>= 1;
			}
			this.Value = res;
		}
	}
}
