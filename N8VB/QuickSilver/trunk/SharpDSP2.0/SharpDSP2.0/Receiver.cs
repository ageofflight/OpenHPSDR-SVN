//==============================================================
//Written by: Philip A Covington, N8VB
//
//This software is licensed under the GNU General Public License
//==============================================================
//receiver.cs
//implements receiver functions for SDR
//==============================================================

using System;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Runtime.Serialization;

namespace SharpDSP
{	
	[Serializable()]
	public class Receiver
	{			
		#region Private members

		internal Filter filter;
		internal LocalOscillator local_osc;
		internal DigitalAGC digital_agc;
		internal NoiseBlanker noise_blanker;
		internal Squelch squelch;
		internal Meter meter;
		internal PowerSpectrum power_spectrum;
		internal AMPLL am_pll;
		internal FMPLL fm_pll;
		internal LMSFilter lms_ANF;
		internal LMSFilter lms_ANR;
		internal OutputMode output_mode;
		internal IQCorrection iq_correction;
		
		#endregion

		#region Constructor

		public Receiver()
		{
			iq_correction = new IQCorrection(this.BlockSize);

			filter = new Filter(this.FFTSize);

			local_osc = new LocalOscillator(this.LocalOscillatorFrequency, 0, 1, this.SampleRate);

			digital_agc = new DigitalAGC(this.AGC_Type, this.AGCHang, this.AGCHangInitial, 
				this.AGCAttackTime, this.AGCMaxGain, this.AGCMinGain, 
				this.AGCLimit, this.AGCInitialGain, this.AGCFixedGain, this.BlockSize);

			noise_blanker = new NoiseBlanker(this.NBThreshold, this.NB2Threshold, this.BlockSize);

			squelch = new Squelch(this.SquelchThreshold, this.SampleRate, this.BlockSize);

			meter = new Meter(this.BlockSize);

			power_spectrum = new PowerSpectrum(this.FFTSize, SharpDSP.WindowType.BLACKMANHARRIS_WINDOW);

			am_pll = new AMPLL(this.SampleRate, this.AMPLLFrequency, this.AMPLLLowLimit, 
				this.AMPLLHighLimit, this.AMPLLBandwidth, this.BlockSize);

			fm_pll = new FMPLL(this.SampleRate, this.FMPLLFrequency, this.FMPLLLowLimit, 
				this.FMPLLHighLimit, this.FMPLLBandwidth, this.BlockSize);

			lms_ANF = new LMSFilter(this.ANFDelay, this.ANFGain, this.ANFLeak, 
				this.ANFTaps, LMSFilterType.Interference, this.BlockSize);

			lms_ANR = new LMSFilter(this.NRDelay, this.NRGain, this.NRLeak, 
				this.NRTaps, LMSFilterType.Noise, this.BlockSize);

			output_mode = new OutputMode(this.BlockSize);						
		}

		#endregion

		#region Public members

		public void ReInitializeRX()
		{
			iq_correction = new IQCorrection(this.BlockSize);

			filter = new Filter(this.FFTSize);

			local_osc = new LocalOscillator(this.LocalOscillatorFrequency, 0, 1, this.SampleRate);

			digital_agc = new DigitalAGC(this.AGC_Type, this.AGCHang, this.AGCHangInitial, 
				this.AGCAttackTime, this.AGCMaxGain, this.AGCMinGain, 
				this.AGCLimit, this.AGCInitialGain, this.AGCFixedGain, this.BlockSize);

			noise_blanker = new NoiseBlanker(this.NBThreshold, this.NB2Threshold, this.BlockSize);

			squelch = new Squelch(this.SquelchThreshold, this.SampleRate, this.BlockSize);

			meter = new Meter(this.BlockSize);

			power_spectrum = new PowerSpectrum(this.FFTSize, SharpDSP.WindowType.BLACKMANHARRIS_WINDOW);

			am_pll = new AMPLL(this.SampleRate, this.AMPLLFrequency, this.AMPLLLowLimit, 
				this.AMPLLHighLimit, this.AMPLLBandwidth, this.BlockSize);

			fm_pll = new FMPLL(this.SampleRate, this.FMPLLFrequency, this.FMPLLLowLimit, 
				this.FMPLLHighLimit, this.FMPLLBandwidth, this.BlockSize);

			lms_ANF = new LMSFilter(this.ANFDelay, this.ANFGain, this.ANFLeak, 
				this.ANFTaps, LMSFilterType.Interference, this.BlockSize);

			lms_ANR = new LMSFilter(this.NRDelay, this.NRGain, this.NRLeak, 
				this.NRTaps, LMSFilterType.Noise, this.BlockSize);

			output_mode = new OutputMode(this.BlockSize);			
		}
						
		public void ProcessAudioStream(ref double[] input_l,
		                               ref double[] input_r,
		                               ref double[] output_l,
		                               ref double[] output_r,
		                               int numsamples)
		{
			if (numsamples != this.BlockSize) { return; }

			double[] inbuffer_l = new double[this.BlockSize];
			double[] inbuffer_r = new double[this.BlockSize];
			double[] outbuffer_l = new double[this.FFTSize];
			double[] outbuffer_r = new double[this.FFTSize];
		
			//copy input buffers into local inbuffers
			MemoryOps.CopyFloatMemory(ref inbuffer_l, ref input_l, this.BlockSize);
			MemoryOps.CopyFloatMemory(ref inbuffer_r, ref input_r, this.BlockSize);

			#region Metering before filter

			meter.DoMeter(ref inbuffer_l, ref inbuffer_r, MeterPosition.PREFILTER);

			#endregion

			#region Power Spectrum Initial

			power_spectrum.PowerSpectrumSignal(ref inbuffer_l, ref inbuffer_r);
			
			#endregion

			#region Do Noiseblankers

			if (this.NBSwitchValue)
			{
				noise_blanker.DoNoiseBlanker(ref inbuffer_l, ref inbuffer_r);
			}

			if (this.NB2SwitchValue)
			{
				noise_blanker.DoSDROMBlanker(ref inbuffer_l, ref inbuffer_r);
			}

			#endregion

			#region IQCorrection

			iq_correction.DoIQCorrection(ref inbuffer_l, ref inbuffer_r, this);

			#endregion

			#region Local Oscillator
            
			local_osc.DoMixer(ref inbuffer_l, 
			                  ref inbuffer_r, 
			                  ref outbuffer_l, 
			                  ref outbuffer_r, 
			                  numsamples);

			#endregion

			#region Power Spectrum before filter

			power_spectrum.PowerSpectrumSignal(ref outbuffer_l, ref outbuffer_r);

			#endregion

			#region Filter
            
			filter.DoFilter(ref outbuffer_l, ref outbuffer_r);
            
			#endregion

			#region Overlap Add

			filter.DoOverlapAdd(ref outbuffer_l, ref outbuffer_r);
                        
			#endregion

			#region Metering after filter

			meter.DoMeter(ref outbuffer_l, ref outbuffer_r, MeterPosition.POSTFILTER);

			#endregion

			#region Power Spectrum after filter

			power_spectrum.PowerSpectrumSignal(ref outbuffer_l, ref outbuffer_r);

			#endregion

			#region Squelch

			squelch.DoSquelch(ref outbuffer_l, ref outbuffer_r);

			#endregion

			#region Do AGC

			digital_agc.DoAGC(ref outbuffer_l, ref outbuffer_r);
			//digital_agc.DoExponentialAGC(buffer_l, buffer_r);
            
			#endregion

			#region Power Spectrum after AGC

			power_spectrum.PowerSpectrumSignal(ref outbuffer_l, ref outbuffer_r);

			#endregion
            
			#region Do Output

			output_mode.DoOutput(ref outbuffer_l, ref outbuffer_r, ref output_l, ref output_r, this);

			#endregion
		}

		public Receiver CloneSubRX(Receiver obj)
		{
			Receiver rcv_new = null;
			Copier copyit = new Copier();
			rcv_new = (Receiver)copyit.CopyObject(obj);
			rcv_new.rx_type = RXType.SubRX;
			return rcv_new;
		}
        
		#endregion

		#region Private members
				
		private void UpdateRXDisplayVars()
		{
			if (this.RXFilterLowCut < 0 && this.RXFilterHighCut <= 0)
			{
				this.RXDisplayHigh = 0;
				if (this.RXFilterLowCut >= -910)
					this.RXDisplayLow = -1000;
				else
					this.RXDisplayLow = (int)(this.RXFilterLowCut * 1.1);
			}
			else if (this.RXFilterLowCut >= 0 && this.RXFilterHighCut > 0)
			{
				this.RXDisplayLow = 0;
				if (this.RXFilterHighCut <= 910)
					this.RXDisplayHigh = 1000;
				else
					this.RXDisplayHigh = (int)(this.RXFilterHighCut * 1.1);
			}
			else if (this.RXFilterLowCut < 0 && this.RXFilterHighCut > 0)
			{
				int max_edge = Math.Max(-this.RXFilterLowCut, this.RXFilterHighCut);
				this.RXDisplayLow = (int)(max_edge * -1.1);
				this.RXDisplayHigh = (int)(max_edge * 1.1);
			}
		}

		#endregion

		#region Properties
		
		private double sq_meter_offset = 0F;
		public double SquelchMeterOffset
		{
			get { return sq_meter_offset; }
			set
			{
				sq_meter_offset= value;
				squelch.MeterOffset = value;				
			}
		}

		private double sq_gain_offset = 0F;
		public double SquelchGainOffset
		{
			get { return sq_gain_offset; }
			set
			{
				sq_gain_offset= value;
				squelch.GainOffset = value;
			}
		}

		private double sq_attn_offset = 0F;
		public double SquelchAttnOffset
		{
			get { return sq_attn_offset; }
			set
			{
				sq_attn_offset= value;
				squelch.AttenuationOffset = value;
			}
		}

		private bool squelch_on = false;
		public bool SquelchOn
		{
			get { return squelch_on; }
			set
			{
				squelch_on = value;
				squelch.SquelchOn = value;
			}
		}

		private double squelch_threshold = 10F;
		public double SquelchThreshold
		{
			get { return squelch_threshold; }
			set
			{
				squelch_threshold = value;
				squelch.Threshold = value;
			}
		}
				
		private double agc_fixed_gain = 10F;
		public double AGCFixedGain
		{
			get { return agc_fixed_gain; }
			set
			{
				agc_fixed_gain = value;
				digital_agc.AgcFixedGain = value;				
			}
		}

		private double agc_max_gain = 60F;
		public double AGCMaxGain
		{
			get { return agc_max_gain; }
			set
			{
				agc_max_gain = value;
				digital_agc.AgcMaxGain = value;
			}
		}

		private double agc_min_gain = 0.1F;
		public double AGCMinGain
		{
			get { return agc_min_gain; }
			set
			{
				agc_min_gain = value;
				digital_agc.AgcMinGain = value;
			}
		}

		private double agc_limit = 701F;
		public double AGCLimit
		{
			get { return agc_limit; }
			set
			{
				agc_limit = value;
				digital_agc.AgcLimit = value;
			}
		}

		private double agc_initial_gain = 1F;
		public double AGCInitialGain
		{
			get { return agc_initial_gain; }
			set
			{
				agc_initial_gain = value;
			}
		}

		private double nb_threshold = 10F;
		public double NBThreshold
		{
			get { return nb_threshold; }
			set
			{
				nb_threshold = value;
				noise_blanker.Threshold = value;
			}
		}

		private double nb2_threshold = 10F;
		public double NB2Threshold
		{
			get { return nb2_threshold; }
			set
			{
				nb2_threshold = value;
				noise_blanker.SDROMThreshold = value;
			}
		}

		private AGCType agc_type = AGCType.agcMedium;
		public AGCType AGC_Type
		{
			get { return agc_type; }
			set
			{
				digital_agc.AgcType = value;
				this.agc_type = value;				
			}
		}

		private int agc_hang = 7;
		public int AGCHang
		{
			get { return agc_hang; }
			set
			{
				agc_hang = value;
				digital_agc.Hang = value;
			}
		}

		private int agc_hang_init = 0;
		public int AGCHangInitial
		{
			get { return agc_hang_init; }
			set
			{
				agc_hang_init = value;
			}
		}

		private int agc_attack_time = 48;
		public int AGCAttackTime
		{
			get { return agc_attack_time; }
			set
			{
				agc_attack_time = value;
				digital_agc.AgcAttackTimeScale = value;
			}
		}

		private double anf_gain = 0.0005F;
		public double ANFGain
		{
			get { return anf_gain; }
			set
			{
				anf_gain = value;
				lms_ANF.AdaptationRate = value;				
			}
		}

		private int anf_delay = 50;
		public int ANFDelay
		{
			get { return anf_delay; }
			set
			{
				anf_delay = value;
				lms_ANF.Delay = value;				
			}			
		}

		private double anf_leak = 0.00001F;
		public double ANFLeak
		{
			get { return anf_leak; }
			set
			{
				anf_leak = value;
				lms_ANF.Leakage = value;				
			}
		}
		
		private int anf_taps = 64;
		public int ANFTaps
		{
			get { return anf_taps; }
			set
			{
				anf_taps = value;
				lms_ANF.AdaptiveFilterSize = value;
			}
		}	
	
		private double nr_gain = 0.0005F;
		public double NRGain
		{
			get { return nr_gain; }
			set
			{
				nr_gain = value;
				lms_ANR.AdaptationRate = value;				
			}
		}

		private int nr_delay = 50;
		public int NRDelay
		{
			get { return nr_delay; }
			set
			{
				nr_delay = value;
				lms_ANR.Delay = value;				
			}			
		}

		private double nr_leak = 0.00001F;
		public double NRLeak
		{
			get { return nr_leak; }
			set
			{
				nr_leak = value;
				lms_ANR.Leakage = value;				
			}
		}
		
		private int nr_taps = 64;
		public int NRTaps
		{
			get { return nr_taps; }
			set
			{
				nr_taps = value;				
				lms_ANR.AdaptiveFilterSize = value;
			}
		}		

		private double osc_freq = 11025F;
		public double LocalOscillatorFrequency
		{
			get { return osc_freq; }
			set
			{
				osc_freq = value;
				local_osc.ChangeFrequency(value);
			}
		}

		private int iq_gain_value = 1;
		public int IQGainValue
		{
			get { return iq_gain_value; }
			set
			{
				iq_gain_value = value;
			}
		}

		private int iq_phase_value = 0;
		public int IQPhaseValue
		{
			get { return iq_phase_value; }
			set
			{
				iq_phase_value = value;
			}
		}
				
		private double samplerate = 48000F;
		public double SampleRate
		{
			get { return samplerate; }
			set
			{
				samplerate = value;
			}
		}
								
		private Window window_type = Window.BLACKMAN4;
		public Window WindowType
		{
			get { return window_type; }
			set
			{
				window_type = value;
			}
		}
		
		private bool binaural_mode_value = false;
		public bool BinauralMode
		{
			get { return binaural_mode_value; }
			set
			{
				binaural_mode_value = value;
			}
		}

		private bool nr_switch_value = false;
		public bool NRSwitchValue
		{
			get { return nr_switch_value; }
			set
			{
				nr_switch_value = value;
			}
		}

		private bool anf_switch_value = false;
		public bool ANFSwitchValue
		{
			get { return anf_switch_value; }
			set
			{
				anf_switch_value = value;
			}
		}

		private bool nb_switch_value = false;
		public bool NBSwitchValue
		{
			get { return nb_switch_value; }
			set
			{
				nb_switch_value = value;
			}
		}

		private bool mnb_switch_value = false;
		public bool NB2SwitchValue
		{
			get { return mnb_switch_value; }
			set
			{
				mnb_switch_value = value;
			}
		}
		
		private int block_size = 2048;
		public int BlockSize
		{
			get { return block_size; }
			set
			{
				block_size = value;
				fft_size = block_size * 2;
			}
		}

		private int fft_size = 4096;
		public int FFTSize
		{
			get { return fft_size; }
			set
			{
				fft_size = value;				
			}
		}

		private double rx_volume_l = 0.25F;
		public double VolumeLeft
		{
			get { return rx_volume_l; }
			set
			{
				rx_volume_l = value;			
			}
		}

		private double rx_volume_r = 0.25F;
		public double VolumeRight
		{
			get { return rx_volume_r; }
			set
			{
				rx_volume_r = value;			
			}
		}
		
		private Mode current_mode = Mode.AM;
		private Mode mode = Mode.AM;
		public Mode CurrentMode
		{
			get { return current_mode; }
			set
			{
				current_mode = value;
				mode = value;				
			}
		}

		private Window current_window = Window.BLACKMAN4;
		public Window CurrentWindow
		{
			get { return current_window; }
			set
			{
				current_window = value;				
			}
		}

		private int rx_display_low = 100;
		public int RXDisplayLow
		{
			get { return rx_display_low; }
			set { rx_display_low = value; }
		}

		private int rx_display_high = 2800;
		public int RXDisplayHigh
		{
			get { return rx_display_high; }
			set { rx_display_high = value; }
		}
		
		private int rx_filter_low_cut = 100;
		public int RXFilterLowCut
		{
			get { return rx_filter_low_cut; }
			set
			{
				rx_filter_low_cut = value;
				this.UpdateRXDisplayVars();
				filter.MakeFilter(this.rx_filter_low_cut, this.rx_filter_high_cut, this.SampleRate, FilterType.BandPass, (WindowType)this.WindowType);                 
			}
		}

		private int rx_filter_high_cut = 2800;
		public int RXFilterHighCut
		{
			get { return rx_filter_high_cut; }
			set
			{
				rx_filter_high_cut = value;
				this.UpdateRXDisplayVars();
				filter.MakeFilter(this.rx_filter_low_cut, this.rx_filter_high_cut, this.SampleRate, FilterType.BandPass, (WindowType)this.WindowType);
			}
		}
				
		private int filter_taps = 2048;
		public int FilterTaps
		{
			get { return filter_taps; }
			set
			{
				filter_taps = value;								
			}
		}
				
		private double current_dds_freq = 0F;
		public double CurrentDDSFreq
		{
			get { return current_dds_freq; }
			set
			{
				current_dds_freq = value;
			}
		}

		private double am_pll_freq = 0F;
		public double AMPLLFrequency
		{
			get { return am_pll_freq; }
			set
			{
				am_pll_freq = value;
			}
		}

		private double am_pll_lo_limit = -500F;
		public double AMPLLLowLimit
		{
			get { return am_pll_lo_limit; }
			set
			{
				am_pll_lo_limit = value;
			}
		}

		private double am_pll_hi_limit = 500F;
		public double AMPLLHighLimit
		{
			get { return am_pll_hi_limit; }
			set
			{
				am_pll_hi_limit = value;
			}
		}

		private double am_pll_bandwidth = 400F;
		public double AMPLLBandwidth
		{
			get { return am_pll_bandwidth; }
			set
			{
				am_pll_bandwidth = value;
			}
		}

		private double fm_pll_freq = 0F;
		public double FMPLLFrequency
		{
			get { return fm_pll_freq; }
			set
			{
				fm_pll_freq = value;
			}
		}

		private double fm_pll_lo_limit = -6000F;
		public double FMPLLLowLimit
		{
			get { return fm_pll_lo_limit; }
			set
			{
				fm_pll_lo_limit = value;
			}
		}

		private double fm_pll_hi_limit = 6000F;
		public double FMPLLHighLimit
		{
			get { return fm_pll_hi_limit; }
			set
			{
				fm_pll_hi_limit = value;
			}
		}

		private double fm_pll_bandwidth = 10000F;
		public double FMPLLBandwidth
		{
			get { return fm_pll_bandwidth; }
			set
			{
				fm_pll_bandwidth = value;
			}
		}

		private RXType rx_type = RXType.MainRX;
		public RXType ReceiverType
		{
			get { return rx_type; }			
		}

		private RXOutputRoute rx_route = RXOutputRoute.None;
		public RXOutputRoute ReceiverOutputRoute
		{
			get { return rx_route; }
			set
			{
				rx_route = value;
			}
		}

//		private PowerSpectrumMode ps_mode = PowerSpectrumMode.OFF;
//		public PowerSpectrumMode PowerSpectrumMode
//		{
//			get { return ps_mode; }
//			set
//			{
//				ps_mode = value;
//				power_spectrum.Mode = value;
//				if (this.CurrentMode == Mode.SPEC)
//					power_spectrum.Mode = PowerSpectrumMode.SPECMODE; //force power_spec position to before filter
//			}
//		}

		public double MeterValue
		{
			get { return meter.MeterValue; }			
		}

		private MeterType meter_type;
		public MeterType Meter_Type
		{
			get { return meter_type; }
			set
			{
				meter_type = value;
				meter.Meter_Type = value;
			}
		}


		#endregion

	}
}
