/*
 * p3d2x3d: This program outputs p3d visualisations in x3d format for the web
 *
 * LT and WvS November 2015
 *
 * To compile: g++ -o p3d2x3d p3d2x3d.c -L/Users/too043/docs/astronomy/software/latest/s2plot-3.2.1/darwin-gnu-x86_64/ -ls2plot -ls2winglut `psrchive --cflags --ldflags`
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
  
  /* Check file contains polarisation information */
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
  //  //double cfreqd = archive->get_centre_frequency();
  string strjname = archive->get_source();
  strjname.copy(jname,80);
  
  /* define and open output file */
  ofstream outfile;
  char outfilename[80];
  string strfilename = jname + string(".x3d");
  strfilename.copy(outfilename,80);
  outfile.open(outfilename);

  /* write xml header */
  outfile << "<X3D width=\"400px\" height=\"400px\">" << endl;
  
  /* Convert to Stokes parameters */
  archive->convert_state(Signal::Stokes);
  
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

  outfile << "<Scene>" << endl;
  outfile << "<Shape>" << endl;
  outfile << "<Appearance>" << endl;
  outfile << "<Material emissiveColor=\"1 1 1\"/>" << endl;
  outfile << "</Appearance>" << endl;
  outfile << "<LineSet vertexCount=\"" << N << "\" containerField=\"geometry\">" << endl;
  outfile << "<Coordinate point=\"";

  /* Loop over the bins */
  for (unsigned ibin=0; ibin < N; ibin++)
    {
      x[ibin] = Qamps[ibin]*10;
      y[ibin] = Uamps[ibin]*10;
      z[ibin] = Vamps[ibin]*10;
      cout << "Qamps=" << x[ibin] << " Uamps=" << y[ibin] << " Vamps=" << z[ibin] << endl;
      outfile << x[ibin] << " " << y[ibin] << " " << z[ibin] << ",";  
      minmax (xmin, xmax, x[ibin]);
      minmax (ymin, ymax, y[ibin]);
      minmax (zmin, zmax, z[ibin]);
    }

  outfile << "\">" << endl;
  outfile << "</Coordinate>" << endl;
  outfile << "</LineSet>" << endl;
  outfile << "</Shape>" << endl;
  outfile << "</Scene>" << endl;
  outfile << "</X3D>" << endl;

  cout << "xmin" << xmin << endl;
  cout << "xmax" << xmax << endl;
  cout << "ymin" << ymin << endl;
  cout << "ymax" << ymax << endl;
  cout << "zmin" << zmin << endl;
  cout << "zmax" << zmax << endl;

  /* close output file */
  outfile.close();

  return 0;
}

