/**
 * @brief Create a web page with an HDR viewer
 * 
 * This file is a part of LuminanceHDR package, based on pfstools.
 * ---------------------------------------------------------------------- 
 * Copyright (C) 2009 Rafal Mantiuk
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * ---------------------------------------------------------------------- 
 * 
 * @author Rafal Mantiuk, <mantiuk@mpi-sb.mpg.de>
 *
 * $Id: pfsouthdrhtml.cpp,v 1.5 2010/06/13 14:45:55 rafm Exp $
 */

#include "hdrhtml.h"

#include <QObject>
#include <cstdlib>
#include <iostream>
#include <string>

#include <stdio.h>
#include "Libpfs/frame.h"
#include "Libpfs/exception.h"
#include "Libpfs/colorspace/colorspace.h"

//TODO
#define PKG_DATADIR "/usr/local/share/luminance-hdr/hdrhtml"

using namespace hdrhtml;
using namespace std;

void generate_hdrhtml(pfs::Frame *frame, 
                     const char *page_name, const char *image_dir, const char *object_output, const char *html_output, 
                     int quality, bool verbose)
{
    const char *page_template = PKG_DATADIR "/hdrhtml_default_templ/hdrhtml_page_templ.html";
    const char *image_template = PKG_DATADIR "/hdrhtml_default_templ/hdrhtml_image_templ.html";
  
    if( quality < 1 || quality > 5 )
        throw pfs::Exception( "The quality must be between 1 (worst) and 5 (best)." );

  
    //TODO
    if( frame == NULL ) {
        return; 
    }

    pfs::Channel *R, *G, *B;
    frame->getXYZChannels( R, G, B );

    int size = frame->getWidth()*frame->getHeight();
    
    pfs::Array2Df X( frame->getWidth(), frame->getHeight() );
    pfs::Array2Df Y( frame->getWidth(), frame->getHeight() );
    pfs::Array2Df Z( frame->getWidth(), frame->getHeight() );
    
    pfs::transformColorSpace( pfs::CS_RGB, R, G, B,
          pfs::CS_XYZ, &X, &Y, &Z );
   
    float *R1 = new float[size];
    float *G1 = new float[size];
    float *B1 = new float[size];
    float *Y1 = new float[size];

    copy(R->begin(), R->end(), R1);
    copy(G->begin(), G->end(), G1);
    copy(B->begin(), B->end(), B1);
    copy(Y.begin(), Y.end(), Y1);
    // Get base_name if needed
    string base_name;
    string tmp_str( page_name );
    
    // Remove extension
    int dot_pos = tmp_str.find_last_of( '.' );
    if( (dot_pos != string::npos) & (dot_pos > 0) )
        tmp_str = tmp_str.substr( 0, dot_pos );

    // Substitute invalid characters
    while( true ) {
        int invalid_pos = tmp_str.find_last_of( "-! #@()[]{}`." );
        if( invalid_pos == string::npos )
            break;
        tmp_str.replace( invalid_pos, 1, 1, '_' );
    }        
      
    base_name = tmp_str;
      
    HDRHTMLSet image_set( NULL, image_dir );
    if (verbose)
        cout << QObject::tr("Adding image ").toStdString() << base_name << QObject::tr(" to the web page").toStdString() << endl;

    image_set.add_image( frame->getWidth(), frame->getHeight(), R1, G1, B1, Y1, base_name.c_str(), quality, verbose );
    image_set.generate_webpage( page_template, image_template, object_output, html_output );
    delete[] R1;
    delete[] G1;
    delete[] B1;
    delete[] Y1;
}

