/*
 * p3dviewer: 3D viewer for polarisation vector visualisation
 * LT August 2013
 *
 * To compile: g++ -o p3dviewer3 p3dviewer3.c -L/Users/too043/docs/astronomy/software/latest/s2plot-3.2.1/darwin-gnu-x86_64/ -ls2plot -ls2winglut `psrchive --cflags --ldflags --pgplot-libs`
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

  archive = Pulsar::Archive::load (argv[1]);
  string src = archive->get_source();
  
  Pulsar::Poincare* poincare = new Pulsar::Poincare;
  poincare->set_longitude(0);
  poincare->set_nstep(5);
  float camlong = poincare->get_longitude();

  cout << "Longitude is set to: " << camlong << endl;
  cout << "Visualising polarisation vectors for: " << src << "..." << endl;

  cpgbeg(0,"?",1,1);
  poincare->plot(archive);
  cpgend();

  return 0;
}
