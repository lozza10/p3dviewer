/*
 * p3dviewer: 3D viewer for polarisation vector visualisation
 * LT August 2013
 *
 * To compile: g++ -o p3dviewer p3dviewer.c -L/Users/too043/docs/astronomy/software/latest/s2plot-3.2.1/darwin-gnu-x86_64/ -ls2plot -ls2winglut `psrchive --cflags --ldflags --pgplot-libs`
 *
 */

#include "Pulsar/Archive.h"
#include "Pulsar/Poincare.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include <iostream>
#include "/Users/too043/docs/astronomy/software/latest/s2plot-3.2.1/src/s2plot.h"
#include <stdio.h>
#include <stdlib.h>

#include <cpgplot.h>

using namespace std;

int main (int argc, char** argv)
{
  if (argc < 2) {
    cerr << "Please specify a filename to process!" << endl;
    return -1;
  }

  Reference::To<Pulsar::Archive> archive;
  //Reference::To<Pulsar::PolnProfile> stokes;

  archive = Pulsar::Archive::load (argv[1]);
  //Pulsar::Poincare* poincare = new Pulsar::Poincare;
  //poincare->set_longitude(53);
  //float camlong = poincare->get_longitude();

  //cout << camlong;
  cout << "Visualising polarisation vectors for " << archive->get_source() << "..." << endl;

  //cpgbeg(0,"?",1,1);
  //poincare->plot(archive);
  //cpgend();

  int N = 2048;
  float x[N], y[N], z[N];
  int xmin = 0;
  int zmin = 0;
  int xmax = 2048;
  int zmax = 2048;
  //int ymin = 23;
  float ymin = 0;
  //int ymax = 26;
  float ymax = 50;
 
  int isubint;
  int ichan;

  s2opend("/?",argc, argv);
  s2swin(xmin,xmax,ymin,ymax,zmin,zmax);
  s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);


  //Reference::To<Pulsar::PolnProfile> profile;
  //Stokes<float> stokes = profile->get_Stokes(archive);

  //const float* amps = profile->get_Profile(1);
  /*for (unsigned ipol=1; ipol < 4; ipol++)
   {
     const float* amps = profile->get_Profile(ipol)->get_amps();

		           float max_amp = *max_element (amps+i_min, amps+i_max);
			       float min_amp = *min_element (amps+i_min, amps+i_max);

			           float abs_min = fabs( min_amp );
				       max_amp = std::max( abs_min, max_amp );
				           max = std::max (max_amp, max);
   }*/


  for (unsigned isub=0; isub < archive->get_nsubint(); isub++)
    {
      Pulsar::Integration* integration = archive->get_Integration(isub);
      for (unsigned ipol=0; ipol < integration->get_npol(); ipol++)
        {
	  for (unsigned ichan=0; ichan < integration->get_nchan(); ichan++)
	    {
	      //Pulsar::Profile* profile = integration->get_Profile (ipol, ichan);
	      //Pulsar::PolnProfile* profile = stokes->get_Profile (ipol);
		    const Pulsar::PolnProfile* profile = get_Stokes (archive, isub, ichan);
	            unsigned nbins = profile->get_nbin();
		    const float* amps = profile->get_Profile(ipol)->get_amps(); 
		    //cout << amps << endl;
		    //float* amps = profile->get_amps();
              //cout << "Nsub: " << isub << " Npol: " << ipol << " Nchan: " << ichan << " Nbins: " << profile->get_nbin() << endl;
	      for (unsigned ibin=0; ibin < N; ibin++)
	        
	     {
		  //cout << amps[ibin] << endl;
		  x[ibin] = ibin;
		  y[ibin] = amps[ibin];
		  cout << y[ibin];
		  z[ibin] = ibin;
	        }
	        s2sci(ipol);
		s2slw(1);
		s2line(N, x, y, z);
	    }
        }
    }

    /*s2opend("/?",argc, argv);
    s2swin(xmin,xmax,ymin,ymax,zmin,zmax);
    s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);*/
    s2sci(S2_PG_YELLOW);
    s2slw(1);
    s2line(N, x, y, z);
    s2pt(N, x, y, z, 1);
    s2show(1);
  
  return 0;
}
