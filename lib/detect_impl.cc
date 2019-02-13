/* -*- c++ -*- */
/* 
 * Copyright 2019 - Quiet Skies LLC -- Glen Langston - glen.i.langston@gmail.com
 * 
 * This is free software;  you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <stdio.h>
#include <time.h> 
#include "detect_impl.h"
#include <iostream>
#include <chrono>

/*
 * Convert Modified Julian Day to calendar date.
 * - Assumes Gregorian calendar.
 * - Adapted from Fliegel/van Flandern ACM 11/#10 p 657 Oct 1968.
 */

void
MjdToDate (long Mjd, long *Year, long *Month, long *Day)
{
  long J, C, Y, M;

  J = Mjd + 2400001 + 68569;
  C = 4 * J / 146097;
  J = J - (146097 * C + 3) / 4;
  Y = 4000 * (J + 1) / 1461001;
  J = J - 1461 * Y / 4 + 31;
  M = 80 * J / 2447;
  *Day = J - 2447 * M / 80;
  J = M / 11;
  *Month = M + 2 - (12 * J);
  *Year = 100 * (C - 49) + Y + J;
}  // end of MjdToDate

namespace gr {
  namespace radio_astro {

    detect::sptr
    detect::make(int vec_length, float dms, float f_obs, float bw, float t_int, int nt)
    {
      return gnuradio::get_initial_sptr
        (new detect_impl(vec_length, dms, f_obs, bw, t_int, nt));
    }

    /*
     * The private constructor
     */
    detect_impl::detect_impl(int vec_length, float dms, float f_obs, float bw, float t_int, int nt)
      : gr::block("detect",
		  gr::io_signature::make(1, 1, sizeof(gr_complex)*vec_length),
		  gr::io_signature::make(1, 1, sizeof(gr_complex)*vec_length)),
        d_vec_length(vec_length),
        d_dms(dms),
        d_f_obs(f_obs),
        d_bw(bw),
        d_t_int(t_int),
        d_nt(nt)
    {}

    /*
     * Our virtual destructor.
     */
    detect_impl::~detect_impl()
    {
    }

    long
    detect_impl::DateToMjd (long Year, long Month, long Day)
    {
      return
        367 * Year
	- 7 * (Year + (Month + 9) / 12) / 4
	- 3 * ((Year + (Month - 9) / 7) / 100 + 1) / 4
        + 275 * Month / 9
        + Day
        + 1721028
	- 2400000;
    } // end of DateToMjd

    double 
    detect_impl::get_mjd()
    {
      double mjd = 0, seconds = 0;
      struct timespec ts;
      int r = clock_gettime(CLOCK_REALTIME, &ts);
      char buff[100];
      time_t now = time(NULL);
      struct tm *ptm = gmtime(&now);

      int year = ptm->tm_year + 1900;
      int month = ptm->tm_mon + 1;
      int day = ptm->tm_mday;

      strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));
      printf("Current time: %s.%09ld UTC\n", buff, ts.tv_nsec);
      
      mjd = DateToMjd( year, month, day);
      seconds =  ptm->tm_sec + (60.*ptm->tm_min) + (3600.*ptm->tm_hour);
      seconds += seconds + (1.e-9*ts.tv_nsec);
      mjd += (seconds/86400.);
      //      printf("MJD: %15.9f + %15.9fs\n", mjd, seconds);

      return mjd;
    } // end of get_mjd()

    void
    detect_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
      unsigned ninputs = ninput_items_required.size();
      for(unsigned int i = 0; i < ninputs; i++)
       	    ninput_items_required[i] = noutput_items;
    }

    void 
    detect_impl::set_dms ( float dms)
    {
      nsigma = dms;
      printf("Input N Sigma: %7.1f\n", nsigma);
      d_dms = dms;
    }
      
    void 
    detect_impl::set_bw ( float bw)
    {
      d_bw = bw;
      printf("Input Bandwidth: %7.1f (MHz)\n", bw);
    }
      
    void 
    detect_impl::set_mode ( int mode)
    {
      event_mode = mode;
      if (event_mode == 0){
	printf("Input Mode: Monitor\n");
      }
      else {
	printf("Input Mode: Detect\n");
      }
	
      d_nt = mode;
    } // end of set_mode()
      
    int
    detect_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];
      unsigned ninputs = ninput_items.size();
      int success;

      success = event(in, out);
      //      if (success == 0)
      //	{
      //	  std::cout << "Processed " << ninputs << " Vectors" << "\n";
      //	}
      //std::cout << out[0*d_dms+ 0] << " " << out[31*d_dms+49] <<"\n";

      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    } // end of detect_impl:: general_work
    

    int
    detect_impl::update_buffer()
    { long vlen = d_vec_length, vlen2 = d_vec_length/2, i = inext2;

      i -= vlen2;  // center peak in middle of output vector

      // if event is well within the circular buffer 
      if ((i > vlen2) && (i < MAX_BUFF - vlen2))
	{ for (long j = 0; j < vlen; j++)
	    { samples[j] = circular[i];
	      i++;
	    }
	}

      // now must reset the buffer to wait for the next event
      bufferfull = 0;
      return 0;
    } // end of update_buffer()
    
    int
    detect_impl::event(const gr_complex *input, gr_complex *output)
    {
      //outbuf = (float *) //create fresh one if necessary
      float n_sigma = d_dms; // translate variables 
      int vlen = d_vec_length;
      gr_complex rp = 0;
      double mag2 = 0, dmjd = 0;
      
      // fill the circular buffer
      for(unsigned int j=0; j < vlen; j++)
	{ rp = input[j];
	  mag2 = (rp.real()*rp.real()) + (rp.imag()*rp.imag());
	  circular[inext] = rp;
	  circular2[inext] = mag2;
	  sum2 += mag2;
	  inext++;
	  if (inext >= MAX_BUFF) // if buffer is full
	    { rms2 = sum2*oneovern;
	      rms = sqrt(rms2);
	      inext = 0;
	      bufferfull = 1;    // flag buffer is now full
	      nsigma_rms = nsigma*nsigma*rms2;
	      sum2 = 0;          // restart rms sum
	    }
	  inext2++;              // update position for search 
	  if (inext2 >= MAX_BUFF) // if at end of circular buffer
	    inext2 = 0;           // go back to beginning
	  if (bufferfull)         // when buffer is full, find peaks
	    {
	      if (circular2[inext2] > nsigma_rms)
		{
		  imax2 = inext2;
		  peak = sqrt(circular2[inext2]);
		  // printf( "N-sigma Peak found: %7.1f\n", peak/rms);
		  // add tags to event
		  add_item_tag(0, // Port number
			       nitems_written(0) + 1, // Offset
			       pmt::mp("PEAK"), // Key
			       pmt::from_double(peak) // Value
			       );
		  add_item_tag(0, // Port number
			       nitems_written(0) + 1, // Offset
			       pmt::mp("RMS"), // Key
			       pmt::from_double(rms) // Value
			       );
		  dmjd = get_mjd();
		  printf("Event MJD: %15.6f; Peak=%8.4f+/-%6.4f\n", dmjd, peak, rms);

		  add_item_tag(0, // Port number
			       nitems_written(0) + 1, // Offset
			       pmt::mp("MJD"), // Key
			       pmt::from_double(dmjd) // Value
			       );

		  update_buffer();
		}
	    } // end if buffere full
	} // end for all samples
	      
      if (! initialized) {
	nsigma = d_dms;
	for (int iii = 0; iii < vlen; iii++)
	  { samples[iii] = input[iii];
	  }
	initialized = 1;     // no need to re-initialize the event
	printf("Input N-Sigma: %7.1f\n", nsigma);
      }

      // always output the last event
      for (int iii = 0; iii < vlen; iii++)
	{ output[iii] = samples[iii];
	}

      return 0;
    } // end of detect_impl::event()

  } /* namespace radio_astro */
} /* namespace gr */

