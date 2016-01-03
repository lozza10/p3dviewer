/*
 * p3dviewer: 3D viewer for polarisation vector visualisation
 * LT August 2013
 *
 * To compile: g++ -o p3dviewer p3dviewer.c -L/Users/too043/docs/astronomy/software/latest/s2plot-3.2.1/darwin-gnu-x86_64/ -ls2plot -ls2winglut `psrchive --cflags --ldflags`
 *
 */


#include "Pulsar/Archive.h"
#include "Pulsar/Poincare.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include <iostream>
#include "/Users/too043/docs/astronomy/software/latest/s2plot-3.2.1/src/s2plot.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;

int main (int argc, char** argv)
{
  if (argc < 2) {
    cerr << "Please specify a filename to process!" << endl;
    return -1;
  }

  Reference::To<Pulsar::Archive> archive;

  archive = Pulsar::Archive::load (argv[1]);

  cout << "Visualising polarisation vectors for " << archive->get_source() << "..." << endl;

  int N = 2048;
  float x[N], y[N], z[N];
  int i;
  int xmin = 0;
  int zmin = 0;
  int xmax = 2448;
  int zmax = 2448;
  int ymin = 23.5;
  int ymax = 26;

  for (unsigned isub=0; isub < archive->get_nsubint(); isub++)
    {
      Pulsar::Integration* integration = archive->get_Integration(isub);
      //for (unsigned ipol=0; ipol < integration->get_npol(); ipol++)
      for (unsigned ipol=0; ipol < 1; ipol++)
        {
	  for (unsigned ichan=0; ichan < integration->get_nchan(); ichan++)
	    {
	      Pulsar::Profile* profile = integration->get_Profile (ipol, ichan);
	      float* amps = profile->get_amps();
              cout << "Nsub: " << isub << " Npol: " << ipol << " Nchan: " << ichan << " Nbins: " << profile->get_nbin() << endl;
	      //for (unsigned ibin=0; ibin < profile->get_nbin(); ibin++)
	      for (unsigned ibin=0; ibin < N; ibin++)
	        {
		  x[ibin] = ibin;
		  y[ibin] = amps[ibin];
		  z[ibin] = ibin;
		  //cout << x[ibin] << " : " << y[ibin] << " : " << z[ibin] << endl;
	        }
	    }
        }
    }

    s2opend("/?",argc, argv);
    s2swin(xmin,xmax,ymin,ymax,zmin,zmax);
    s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);
    s2sci(S2_PG_YELLOW);
    s2slw(1);
    s2line(N, x, y, z);
    //s2pt(N, x, y, z, 1);
    s2show(1);
  
  return 0;
}
