/*
 * p3d2x3d: This program outputs p3d visualisations in x3d format for the web
 *
 * LT and WvS November 2015
 *
 * To compile:
 * $ g++ -o p3d2x3d p3d2x3d.c -stdlib=libstdc++ `psrchive --cflags --ldflags`
 * *
 */

#include "Pulsar/Archive.h"
#include "Pulsar/Poincare.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include <iostream>
#include <fstream>
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

  /* Defaults */  
  int i, isub = 0, ichan = 0;
  float xmin = 0, ymin = 0, zmin = 0;
  float xmax = 0, ymax = 0, zmax = 0;

  /* Instantiate the archive object */
  Reference::To<Pulsar::Archive> archive;

  /* Load the archive file */
  char * infile = argv[1];
  archive = Pulsar::Archive::load(infile);
  
  /* Remove baseline */
  archive->remove_baseline();
  
  /* Perform frequency scrunch to single channel */
  archive->fscrunch();
  
  /* Perform time scrunch to single subintegration*/
  archive->tscrunch();
  
  /* Check file contains polarisation information */
  if (archive->get_npol() != 4)
  {
    cerr << "WARNING: data file does not contain full polarisation data" << endl;
    return -1;
  }

  /* Convert to Stokes parameters */
  archive->convert_state(Signal::Stokes);
  
  /* Instantiate the subintegration object */
  Pulsar::Integration* integration = archive->get_Integration(isub);

  /* Get all Stokes parameters */
  Pulsar::Profile* Q = integration->get_Profile (1, ichan);
  Pulsar::Profile* U = integration->get_Profile (2, ichan);
  Pulsar::Profile* V = integration->get_Profile (3, ichan);

  /* Get the amplitudes for each Stokes parameter */
  float* Qamps = Q->get_amps();
  float* Uamps = U->get_amps();
  float* Vamps = V->get_amps();
  
  /* Get centre frequency */
  double cfreq = archive->get_centre_frequency();

  /* Get source name */
  string strjname = archive->get_source(); 

  /* Get number of bins */
  int N = archive->get_nbin();
  
  /* Define arrays */  
  float x[N], y[N], z[N];
  
  /* Define and open output file */
  ofstream outfile;
  char outfilename[80];
  string strfilename = string("J") + strjname + string(".x3d");
  strfilename.copy(outfilename,80);
  outfile.open(outfilename);
 
  /* Print file details to stdout */
  cout << "-- p3d2x3d --" << endl;
  cout << "Input file name: " << infile << endl;
  cout << "X3D output file name: " << strfilename << endl;
  cout << "Source: " << strjname << endl;
  cout << "Centre frequency: " << cfreq << endl;
  cout << "NBIN: " << N << endl;
  cout << "NCHAN: " << ichan << endl;
  cout << "NSUB: " << isub << endl;
  
  /* Write xml header */
  outfile << "<X3D profile=\"Immersive\" version=\"3.2\" width=\"400px\" height=\"400px\">" << endl;
  
  /* Output the xml for the x3d dom */
  outfile << "<Scene>" << endl;
  outfile << "<Shape>" << endl;
  outfile << "<Sphere radius=\"4.7\"/>" << endl;
  outfile << "<Appearance>" << endl;
  outfile << "<Material transparency=\"0.89\"/>" << endl;
  outfile << "</Appearance>" << endl;
  outfile << "</Shape>" << endl;
  outfile << "<Group>" << endl;
  /* Y-axis arrow and label */ 
  outfile << "<Group DEF=\"ArrowBlue\">" << endl;
  outfile << "<Shape>" << endl;
  outfile << "<Cylinder DEF=\"ArrowCylinder\" radius=\".005\" top=\"false\"/>" << endl;
  outfile << "<Appearance DEF=\"Blue\">" << endl;
  outfile << "<Material diffuseColor=\"0 0 1\" emissiveColor=\"0 0 1\"/>" << endl;
  outfile << "</Appearance>" << endl;
  outfile << "</Shape>" << endl; 
  outfile << "<Transform translation=\"0 1 0\">" << endl;
  outfile << "<Shape>" << endl;
  outfile << "<Cone DEF=\"ArrowCone\" bottomRadius=\".05\" height=\".1\"/>" << endl;
  outfile << "<Appearance USE=\"Blue\"/>" << endl;
  outfile << "</Shape>" << endl;
  outfile << "</Transform>" << endl;
  outfile << "</Group>" << endl;
  outfile << "<Transform translation=\"0 0.8 0\">" << endl;
  outfile << "<Billboard>" << endl;
  outfile << "<Shape>" << endl;
  outfile << "<Appearance USE=\"Blue\"/>" << endl;
  outfile << "<Text string='\"V\"'>" << endl;
  outfile << "<FontStyle DEF=\"LABEL_FONT\" family='\"SANS\"' justify='\"MIDDLE\" \"MIDDLE\"' size=\".2\"/>" << endl;
  outfile << "</Text>" << endl;
  outfile << "</Shape>" << endl;
  outfile << "</Billboard>" << endl;
  outfile << "</Transform>" << endl;
  outfile << "</Group>" << endl;
  outfile << "<Transform rotation=\"0 0 1 -1.57079\">" << endl;
  /* X-axis arrow and label */
  outfile << "<Group>" << endl;
  outfile << "<Group DEF=\"ArrowGreen\">" << endl;
  outfile << "<Shape>" << endl;
  outfile << "<Cylinder USE=\"ArrowCylinder\"/>" << endl; 
  outfile << "<Appearance DEF=\"Green\">" << endl;
  outfile << "<Material diffuseColor=\"0 1 0\" emissiveColor=\"0 1 0\"/>" << endl;
  outfile << "</Appearance>" << endl;
  outfile << "</Shape>" << endl;
  outfile << "<Transform translation=\"0 1 0\">" << endl;
  outfile << "<Shape>" << endl;
  outfile << "<Cone USE=\"ArrowCone\"/>" << endl; 
  outfile << "<Appearance USE=\"Green\"/>" << endl;
  outfile << "</Shape>" << endl;
  outfile << "</Transform>" << endl;
  outfile << "</Group>" << endl;
  outfile << "<Transform rotation=\"0 0 1 1.57079\" translation=\"0.3 1.3 0\">" << endl;
  outfile << "<Billboard>" << endl;
  outfile << "<Shape>" << endl;
  outfile << "<Appearance USE=\"Green\"/>" << endl; 
  outfile << "<Text string='\"U\"'>" << endl;
  outfile << "<FontStyle USE=\"LABEL_FONT\"/>" << endl;
  outfile << "</Text>" << endl;
  outfile << "</Shape>" << endl;
  outfile << "</Billboard>" << endl;
  outfile << "</Transform>" << endl;
  outfile << "</Group>" << endl;
  outfile << "</Transform>" << endl;
  outfile << "<Transform rotation=\"1 0 0 1.57079\">" << endl;
  outfile << "<Group>" << endl;
  /* Z-axis arrow and label */
  outfile << "<Group DEF=\"ArrowRed\">" << endl;
  outfile << "<Shape>" << endl;
  outfile << "<Cylinder USE=\"ArrowCylinder\"/>" << endl;
  outfile << "<Appearance DEF=\"Red\">" << endl;
  outfile << "<Material diffuseColor=\"1 0 0\" emissiveColor=\"1 0 0\"/>" << endl;
  outfile << "</Appearance>" << endl;
  outfile << "</Shape>" << endl;
  outfile << "<Transform translation=\"0 1 0\">" << endl;
  outfile << "<Shape>" << endl;
  outfile << "<Cone USE=\"ArrowCone\"/>" << endl;
  outfile << "<Appearance USE=\"Red\"/>" << endl;
  outfile << "</Shape>" << endl;
  outfile << "</Transform>" << endl;
  outfile << "</Group>" << endl;
  outfile << "<Transform rotation=\"1 0 0 -1.57079\" translation=\"0 1.3 0.3\">" << endl;
  outfile << "<Billboard>" << endl;
  outfile << "<Shape>" << endl;
  outfile << "<Appearance USE=\"Red\"/>" << endl;
  outfile << "<Text string='\"Q\"'>" << endl;
  outfile << "<FontStyle USE=\"LABEL_FONT\"/>" << endl;
  outfile << "</Text>" << endl;
  outfile << "</Shape>" << endl;
  outfile << "</Billboard>" << endl;
  outfile << "</Transform>" << endl;
  outfile << "</Group>" << endl;
  outfile << "</Transform>" << endl;
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
      outfile << x[ibin] << " " << y[ibin] << " " << z[ibin] << ",";  
      minmax (xmin, xmax, x[ibin]);
      minmax (ymin, ymax, y[ibin]);
      minmax (zmin, zmax, z[ibin]);
    }

  /* Complete the x3d dom */
  outfile << "\">" << endl;
  outfile << "</Coordinate>" << endl;
  outfile << "</LineSet>" << endl;
  outfile << "</Shape>" << endl;
  outfile << "</Scene>" << endl;
  outfile << "</X3D>" << endl;

  /* Close output file */
  outfile.close();

  return 0;
}

