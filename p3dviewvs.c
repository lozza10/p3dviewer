/*
 * p3dviewer: 3D viewer for polarisation vector visualisation
 * LT and WvS November 2014
 *
 * To compile: g++ -o p3dviewer p3dviewer.c -L/Users/too043/docs/astronomy/software/latest/s2plot-3.2.1/darwin-gnu-x86_64/ -ls2plot -ls2winglut `psrchive --cflags --ldflags`
 *
 * To do:
 * Plot QUV vectors centred at 0:0:0
 * Convert to web VML language ?
 * Add plot titles and axes for s2plot
 *
 */


#include "Pulsar/Archive.h"
#include "Pulsar/Poincare.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include <iostream>
#include <fstream>
#include "/Users/too043/docs/astronomy/software/latest/s2plot-3.2.1/src/s2plot.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;

/* Function to get min and max value */
void minmax (float& min, float& max, float val)
{
  if (val < min)
    min = val;
  if (val > max)
    max = val;
}

/* Main */
int main (int argc, char** argv)
{
  if (argc < 2) {
    cerr << "Please specify a filename to process!" << endl;
    return -1;
  }

  /* open output file */
  ofstream outfile;
  outfile.open ("example.x3d");

  /* write xml header */
  outfile << "<!DOCTYPE X3D PUBLIC \"ISO//Web3D//DTD X3D 3.2//EN\"" << endl;
  outfile << "http://www.web3d.org/specifications/x3d-3.2.dtd>" << endl;
  outfile << "<X3D profile=\"Interchange\" version=\"3.2\"" << endl;
  outfile << "xmlns:xsd=\"http://www.w3.org/2001/XMLSchema-instance\"" << endl;
  outfile << "xsd:noNamespaceSchemaLocation=\"http://www.web3d.org/specifications/x3d-3.2.xsd\">" << endl;

  /* Instantiate the archive object */
  Reference::To<Pulsar::Archive> archive;

  /* Load the archive file */
  archive = Pulsar::Archive::load (argv[1]);
  /* Remove baseline */
  archive->remove_baseline();
  /* Perform frequency scrunch to single channel */
  archive->fscrunch ();
  /* Perform time scrunch to single subintegration*/
  archive->tscrunch ();
  
  /* Check file contains polaisation information */
  if (archive->get_npol() != 4)
  {
    cerr << "WARNING: data file does not contain full poln data" << endl;
    return -1;
  }

  int N = 2048;
  int i, isub = 0, ichan = 0;
  float x[N], y[N], z[N];
  float xmin = 0, ymin = 0, zmin = 0;
  float xmax = 0, ymax = 0, zmax = 0;
  char jname[80];
  //char cfreq[80];
  //double cfreqd = archive->get_centre_frequency();
  string strjname = archive->get_source();
  strjname.copy(jname,80);
  //str.copy(jname,80);

  /* Convert to Stokes parameters */
  archive->convert_state(Signal::Stokes);
  
  /* Start output */
  cout << "Visualising polarisation vectors for " << jname << "..." << endl;
  
  /* Instantiate the subintegration object */
  Pulsar::Integration* integration = archive->get_Integration(isub);

  /* Get all Stokes parameters */
  Pulsar::Profile* Q = integration->get_Profile (1, ichan);
  Pulsar::Profile* U = integration->get_Profile (2, ichan);
  Pulsar::Profile* V = integration->get_Profile (3, ichan);

  /* Get number of bins */
  N = archive->get_nbin();
  
  /* Get the amplitudes for each Stokes parameter */
  float* Qamps = Q->get_amps();
  float* Uamps = U->get_amps();
  float* Vamps = V->get_amps();

  cout << "NBIN=" << N << endl;
  cout << "NCHAN=" << ichan << endl;
  cout << "NSUB=" << isub << endl;

  /* Loop over the bins */
  for (unsigned ibin=0; ibin < N; ibin++)
    {
      x[ibin] = Qamps[ibin];
      y[ibin] = Uamps[ibin];
      z[ibin] = Vamps[ibin];
      cout << "Qamps=" << x[ibin] << " Uamps=" << y[ibin] << " Vamps=" << y[ibin] << endl;
      minmax (xmin, xmax, x[ibin]);
      minmax (ymin, ymax, y[ibin]);
      minmax (zmin, zmax, z[ibin]);
    }

  cout << "xmin" << xmin << endl;
  cout << "xmax" << xmax << endl;
  cout << "ymin" << ymin << endl;
  cout << "ymax" << ymax << endl;
  cout << "zmin" << zmin << endl;
  cout << "zmax" << zmax << endl;

  /* Call s2plot */
  s2opend("/?",argc, argv);
  s2swin(xmin,xmax,ymin,ymax,zmin,zmax);
  //s2box("BCDET",0,0,"BCDET",0,0,"BCDET",0,0);
  s2lab("","","",jname);
  //pushVRMLname("BOX");                 /* Create a new VRML object name */
  s2sci(S2_PG_YELLOW);
  s2slw(1);
  s2line(N, x, y, z);
  
  /* stokes V (blue arrow) */
  s2sci(S2_PG_BLUE);
  s2sah(1, 30, 0.7);			/* Set arrow head style */
  s2sch(5);
  s2arro(0.0,0.0,0.0, 0.0,ymax,0.0);
  
  /* stokes Q (red arrow) */
  s2sci(S2_PG_RED);
  s2sah(1, 30, 0.7);                    /* Set arrow head style */
  s2sch(5);
  s2arro(0.0,0.0,0.0, xmin,0.0,zmax);
  
  /* stokes U (green arrow) */
  s2sci(S2_PG_GREEN);
  s2sah(1, 30, 0.7);                    /* Set arrow head style */
  s2sch(5);
  s2arro(0.0,0.0,0.0, xmax,0.0,zmax);
  
  /* key */
  //ns2text(0,0,0, 0.4,0.3,0.2, 0.2,0.8,0.9, 1.0,0.0,0.4, "V");
  //ns2text(0,0,0, 0,0,0, 0,0,0, 0,0,255, "Q");
  //ns2text(0,0,0, 0,0,0, 0,0,0, 0,0,255, "U");
  //s2textxy(0.1,0.2,0.3, "V"); 

  //pushVRMLname("ANON");                /* Restore default VRML object name */
  s2show(1);
  
  /* close output file */
  outfile.close();

  return 0;
}

