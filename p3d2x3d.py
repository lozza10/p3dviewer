#!/usr/bin/env python

"""p3d2x3d.py

This program writes HTML and X3D format files of PSRCHIVE 'p3d-style'
polarisation data for 3D visualsation in X3D enabled web browsers 
and X3D viewers

Copyright (C) 2016  Lawrence Toomey

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import psrchive
import argparse
from lxml import etree as ET

isub = 0
ibin = 0
ichan = 0
nbin = 0
str_points = ''
red_rgb = '1 0 0'
green_rgb = '0 1 0'
blue_rgb = '0 0 1'
white_rgb = '1 1 1' 

def draw(root_element, shape_type, emissive_colour, diffuse_colour, \
         transparency, height, radius):
    """Function to render a shape in the X3DOM
    
    Renders a given shape type in a given parent element of the X3DOM

    Arguments:
        root_element: Parent element in which to place 
                      the shape sub element
        shape_type: Shape type [options: Sphere|Cylinder|Cone]
        emissive_colour: Emissive colour [R G B]
        diffuse_colour: Diffuse colour [R G B]
        transparency: Amount of tranparency [0(None):1(transparent)]
        height: Element height
        radius: Element radius

    Returns:
        An X3D element containing the required shape, as an xml string
    """

    s_elem = ET.SubElement(root_element, 'Shape')
    
    if shape_type == 'Sphere':
        s_sub_elem = ET.SubElement(s_elem, shape_type, radius=radius)
    elif shape_type == 'Cylinder':
        s_sub_elem = ET.SubElement(s_elem, shape_type, radius=radius, \
                                   top='TRUE')
    elif shape_type == 'Cone':
        s_sub_elem = ET.SubElement(s_elem, shape_type, bottomRadius=radius, \
                                   height=height)
     
    s_app = ET.SubElement(s_elem, 'Appearance')
    s_material = ET.SubElement(s_app, 'Material', \
                               emissiveColor=emissive_colour, \
                               diffuseColor=diffuse_colour, \
                               transparency=transparency)
    
    return ET.tostring(s_elem)

def label(root_element, shape_type, emissive_colour, diffuse_colour, \
          transparency, string, font_type, font_size, justify):
    """Function to add a label to an element in the X3DOM
    
    Adds a text label to a given parent element in the X3DOM

    Arguments:
        root_element: Parent element in which to place 
                      the label sub element
        shape_type: Shape type [options: Text]
        emissive_colour: Emissive colour [R G B]
        diffuse_colour: Diffuse colour [R G B]
        transparency: Amount of tranparency [0(None):1(transparent)]
        string: Label text string
        font_type: Font family
        font_size: Font size
        justify: Label position

    Returns:
        An X3D element containing a label, as an xml string            
    """

    l_elem = ET.SubElement(root_element, 'Shape')
    l_sub_elem = ET.SubElement(l_elem, 'Text', string=string)
    l_sub_elem_font = ET.SubElement(l_sub_elem, 'FontStyle', \
                                    family=font_type, \
                                    size=font_size, \
                                    justify=justify)
    l_app = ET.SubElement(l_elem, 'Appearance')
    l_material = ET.SubElement(l_app, 'Material', \
                               emissiveColor=emissive_colour, \
                               diffuseColor=diffuse_colour, \
                               transparency=transparency)

    return ET.tostring(l_elem)

"""Parse command-line arguments"""
parser = argparse.ArgumentParser(usage='p3d2x3d.py file\n'
         '\nThis program writes HTML and X3D format files of PSRCHIVE p3d-style\n'
         'polarisation data for 3D visualsation in X3D enabled web browsers\n' 
         'and X3D viewers\n'
         '\nCopyright (C) 2016  Lawrence Toomey\n'
         'This program comes with ABSOLUTELY NO WARRANTY.\n'
         'This is free software, and you are welcome to redistribute it\n'
         'under certain conditions.\n')
parser.add_argument('file', type=argparse.FileType('r'))
args = parser.parse_args()

"""Instantiate the archive object and load the input file"""
in_file = args.file.name
try:
    ar = psrchive.Archive_load(in_file)
except:
    print 'ERROR: problem loading file %s. Exiting.' %in_file

"""Check the file contains full polarisation data"""
npol = ar.get_npol()
if npol != 4:
    print 'ERROR: file %s does not contain' \
          ' full polarisation data. Exiting.' %in_file
    quit()

"""Remove the baseline"""
ar.remove_baseline()

"""Frequency scrunch to a single channel"""
ar.fscrunch()
    
"""Time scrunch to a single subintegration"""
ar.tscrunch()
	
"""Convert to Stokes parameters"""
ar.convert_state('Stokes')
    
"""Instantiate the subintegration object"""
subint = ar.get_Integration(isub)

"""Get the Stokes Q, U and V parameters"""
Q = subint.get_Profile(1, ichan)
U = subint.get_Profile(2, ichan)
V = subint.get_Profile(3, ichan)

"""Get the amplitudes for each Stokes parameter"""
Qamps = Q.get_amps()
Uamps = U.get_amps()
Vamps = V.get_amps()
			  
"""Get the centre frequency"""
cfreq = ar.get_centre_frequency()

"""Get the source name"""
src = ar.get_source()

"""Get the number of bins"""
nbin = ar.get_nbin()

"""Define the output file names and open for writing"""
x3d_file_name = src + '.x3d'
html_file_name = src + '.html'
x3d_file = open(x3d_file_name, 'w')
html_file = open(html_file_name, 'w')

"""Print file details to stdout"""
print '\n-- p3d2x3d --'
print 'Input file name:        %s' %in_file
print 'Source:                 %s' %src
print 'Centre frequency:       %f' %cfreq
print 'No. of polarisations:   %d' %npol
print 'No. of bins:            %d' %nbin

"""Build the X3DOM"""
root = ET.Element('X3D', profile='Immersive', version='3.2')
scene = ET.SubElement(root, 'Scene')

"""Add the title"""
title_label_transform = ET.SubElement(scene, 'Transform', \
                                      translation='0 2 0')
title_billboard = ET.SubElement(title_label_transform, 'Billboard')
label(title_billboard, 'Text', white_rgb, white_rgb, '0', \
      '"'  + src + ' ' + str(cfreq) + 'MHz"', '"SANS"', '0.2', \
      '"MIDDLE" "MIDDLE"')

"""Draw the cylinder for the Stokes U (X-axis) vector"""
x_transform = ET.SubElement(scene, 'Transform', rotation='0 0 1 -1.57079')
x_group = ET.SubElement(x_transform, 'Group')
x_group_def = ET.SubElement(x_group, 'Group')
draw(x_group_def, 'Cylinder', green_rgb, green_rgb, '0', '0', '0.005')

"""Draw the cone for the Stokes U vector"""
x_cone_transform = ET.SubElement(x_group_def, 'Transform', translation='0 1 0')
draw(x_cone_transform, 'Cone', green_rgb, green_rgb, '0', '0.1', '0.05')

"""Add the label to the Stokes U vector"""
x_label_transform = ET.SubElement(x_group, 'Transform', \
                                  rotation='0 0 1 1.57079', \
                                  translation='0.3 1.3 0')
x_billboard = ET.SubElement(x_label_transform, 'Billboard')
label(x_billboard, 'Text', green_rgb, green_rgb, '0', '"U"', '"SANS"', '0.2', \
      '"MIDDLE" "MIDDLE"')

"""Draw the cylinder for the Stokes V (Y-axis) vector"""
y_group = ET.SubElement(scene, 'Group')
y_group_def = ET.SubElement(y_group, 'Group')
draw(y_group_def, 'Cylinder', blue_rgb, blue_rgb, '0', '0', '0.005')

"""Draw the cone for the Stokes V vector"""
y_cone_transform = ET.SubElement(y_group_def, 'Transform', translation='0 1 0')
draw(y_cone_transform, 'Cone', blue_rgb, blue_rgb, '0', '0.1', '0.05')

"""Add the label to the Stokes V vector"""
y_label_transform = ET.SubElement(y_group, 'Transform', \
                                  translation='0 1.2 0')
y_billboard = ET.SubElement(y_label_transform, 'Billboard')
label(y_billboard, 'Text', blue_rgb, blue_rgb, '0', '"V"', '"SANS"', '0.2', \
      '"MIDDLE" "MIDDLE"')

"""Draw the cylinder for the Stokes Q (Z-axis) vector"""
z_transform = ET.SubElement(scene, 'Transform', rotation='1 0 0 1.57079')
z_group = ET.SubElement(z_transform, 'Group')
z_group_def = ET.SubElement(z_group, 'Group')
draw(z_group_def, 'Cylinder', red_rgb, red_rgb, '0', '0', '0.005')

"""Draw the cone for the Stokes Q vector"""
z_cone_transform = ET.SubElement(z_group_def, 'Transform', translation='0 1 0')
draw(z_cone_transform, 'Cone', red_rgb, red_rgb, '0', '0.1', '0.05')

"""Add the label to the Stokes Q vector"""
z_label_transform = ET.SubElement(z_group, 'Transform', \
                                  rotation='1 0 0 -1.57079', \
                                  translation='0 1.3 0.3')
z_billboard = ET.SubElement(z_label_transform, 'Billboard')
label(z_billboard, 'Text', red_rgb, red_rgb, '0', '"Q"', '"SANS"', '0.2', \
      '"MIDDLE" "MIDDLE"')

"""Plot the Stokes Q, U and V polarisation vectors"""
str_nbin = str(nbin)
pol_shape = ET.SubElement(scene, 'Shape')
pol_app = ET.SubElement(pol_shape, 'Appearance')
pol_material = ET.SubElement(pol_app, 'Material', emissiveColor=white_rgb)
line_set = ET.SubElement(pol_shape, 'LineSet', vertexCount=str_nbin, \
                         containerField='geometry')

"""Loop over the bins"""
x = range(nbin)
y = range(nbin)
z = range(nbin)

for ibin in range(nbin):
    x[ibin] = Qamps[ibin]*10
    y[ibin] = Uamps[ibin]*10
    z[ibin] = Vamps[ibin]*10
    x_coord = str(x[ibin])
    y_coord = str(y[ibin])
    z_coord = str(z[ibin])
    str_points = str_points + ' ' + x_coord + \
                 ' ' + y_coord + ' ' + z_coord + ','

coord = ET.SubElement(line_set, 'Coordinate', point=str_points) 

"""Write the X3DOM to the output x3d file"""
tree = ET.ElementTree(root)
tree.write(x3d_file, pretty_print=True, xml_declaration=False)

"""Write the HTML markup to the output html file"""
content = """<!DOCTYPE html>
<html>
  <head>
    <meta http-equiv='Content-Type' content='text/html;charset=utf-8' />
    <title>""" + src + """</title>
    <script type='text/javascript' src='http://www.x3dom.org/download/x3dom.js'></script>
    <style>
      body {background-color: black;}
    </style>
  </head>
  <body>
    <x3d width='100%' height='50%'> 
      <scene>
        <inline url='""" + x3d_file_name + """'></inline> 
      </scene> 
    </x3d> 
  </body>
</html>
"""
html_file.write(content)
    
"""Close the output files"""
print 'X3D output written to:  %s'  %x3d_file_name
print 'HTML output written to: %s\n' %html_file_name
x3d_file.close()
html_file.close()
