################################################################################
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
################################################################################

set(WITH_GDAL ON CACHE BOOL "GDAL on" FORCE)
set(WITH_GDAL_EXTERNAL OFF CACHE BOOL "GDAL external on")

set(WITH_OpenSSL ON CACHE BOOL "OpenSSL on" FORCE)
set(WITH_OpenSSL_EXTERNAL OFF CACHE BOOL "OpenSSL external on")
set(WITH_ICONV ON CACHE BOOL "iconv on")
set(WITH_ICONV_EXTERNAL OFF CACHE BOOL "iconv external on")
set(WITH_CURL ON CACHE BOOL "CURL on" FORCE)
set(WITH_CURL_EXTERNAL OFF CACHE BOOL "CURL external on")
set(WITH_GEOS ON CACHE BOOL "GEOS on" FORCE)
set(WITH_GEOS_EXTERNAL OFF CACHE BOOL "GEOS external on")
set(WITH_PROJ4 ON CACHE BOOL "PROJ4 on" FORCE)
set(WITH_PROJ4_EXTERNAL OFF CACHE BOOL "PROJ4 external on")
set(WITH_EXPAT ON CACHE BOOL "EXPAT on" FORCE)
set(WITH_EXPAT_EXTERNAL OFF CACHE BOOL "EXPAT external on")
set(WITH_JSONC ON CACHE BOOL "JSONC on" FORCE)
set(WITH_JSONC_EXTERNAL OFF CACHE BOOL "JSONC external on")
set(WITH_ZLIB ON CACHE BOOL "ZLIB on" FORCE)
set(WITH_ZLIB_EXTERNAL OFF CACHE BOOL "ZLIB external on")

set(WITH_TIFF ON CACHE BOOL "TIFF on" FORCE)
set(WITH_TIFF_EXTERNAL OFF CACHE BOOL "TIFF external on") 
set(WITH_JPEG ON CACHE BOOL "JPEG on" FORCE)
set(WITH_JPEG_EXTERNAL OFF CACHE BOOL "JPEG external on")
set(WITH_GeoTIFF ON CACHE BOOL "GeoTIFF on" FORCE)
set(WITH_GeoTIFF_EXTERNAL OFF CACHE BOOL "GeoTIFF external on")

find_anyproject(GDAL REQUIRED CMAKE_ARGS 

                    -DGDAL_BUILD_APPS=OFF
                    -DGDAL_BUILD_DOCS=OFF

                    -DENABLE_PLSCENES=OFF
                    -DENABLE_AAIGRID_GRASSASCIIGRID=OFF
                    -DENABLE_ADRG_SRP=OFF
                    -DENABLE_AIG=OFF
                    -DENABLE_AIRSAR=OFF
                    -DENABLE_ARG=OFF
                    -DENABLE_BLX=OFF
                    -DENABLE_BMP=OFF
                    -DENABLE_BSB=OFF
                    -DENABLE_CALS=OFF
                    -DENABLE_CEOS=OFF
                    -DENABLE_CEOS2=OFF
                    -DENABLE_COASP=OFF
                    -DENABLE_COSAR=OFF
                    -DENABLE_CTG=OFF
                    -DENABLE_DIMAP=OFF
                    -DENABLE_DTED=OFF
                    -DENABLE_E00GRID=OFF
                    -DENABLE_ELAS=OFF
                    -DENABLE_ENVISAT=OFF
                    -DENABLE_ERS=OFF
                    -DENABLE_FIT=OFF
                    -DENABLE_GFF=OFF
                    -DENABLE_GRIB=OFF
                    -DENABLE_GSAG_GSBG_GS7BG=OFF
                    -DENABLE_GXF=OFF
                    -DENABLE_HF2=OFF
                    -DENABLE_IDRISI_RASTER=OFF
                    -DENABLE_ILWIS=OFF
                    -DENABLE_INGR=OFF
                    -DENABLE_IRIS=OFF
                    -DENABLE_JAXAPALSAR=OFF
                    -DENABLE_JDEM=OFF
                    -DENABLE_KMLSUPEROVERLAY=OFF
                    -DENABLE_L1B=OFF
                    -DENABLE_LEVELLER=OFF
                    -DENABLE_MAP=OFF
                    -DENABLE_MBTILES=OFF
                    -DENABLE_MSGN=OFF
                    -DENABLE_NGSGEOID=OFF
                    -DENABLE_NITF_RPFTOC_ECRGTOC=OFF
                    -DENABLE_NWT=OFF
                    -DENABLE_OZI=OFF
                    -DENABLE_PDS_ISIS2_ISIS3_VICAR=OFF
                    -DENABLE_PLMOSAIC=OFF
                    -DENABLE_PNG=OFF
                    -DENABLE_POSTGISRASTER=OFF
                    -DENABLE_R=OFF
                    -DENABLE_RASTERLITE=OFF
                    -DENABLE_RIK=OFF
                    -DENABLE_RMF=OFF
                    -DENABLE_RS2=OFF
                    -DENABLE_SAGA=OFF
                    -DENABLE_SDTS_RASTER=OFF
                    -DENABLE_SGI=OFF
                    -DENABLE_SRTMHGT=OFF
                    -DENABLE_TERRAGEN=OFF
                    -DENABLE_TIL=OFF
                    -DENABLE_TSX=OFF
                    -DENABLE_USGSDEM=OFF
                    -DENABLE_WCS=OFF
                    -DENABLE_WMS=OFF
                    -DENABLE_WMTS=OFF
                    -DENABLE_XPM=OFF
                    -DENABLE_XYZ=OFF
                    -DENABLE_ZMAP=OFF
                    -DENABLE_AERONAVFAA=OFF
                    -DENABLE_ARCGEN=OFF
                    -DENABLE_AVC=OFF
                    -DENABLE_BNA=OFF
                    -DENABLE_CARTO=OFF
                    -DENABLE_CLOUDANT=OFF
                    -DENABLE_COUCHDB=OFF
                    #    -DENABLE_CSV=OFF
                    -DENABLE_CSW=OFF
                    -DENABLE_DGN=OFF
                    -DENABLE_DXF=OFF
                    -DENABLE_EDIGEO=OFF
                    -DENABLE_ELASTIC=OFF
                    -DENABLE_GEOCONCEPT=OFF
                    -DENABLE_GEORSS=OFF
                    -DENABLE_GFT=OFF
                    -DENABLE_GML=OFF
                    -DENABLE_GMT=OFF
                    -DENABLE_GPSBABEL=OFF
                    -DENABLE_GPX=OFF
                    -DENABLE_GTM=OFF
                    -DENABLE_HTF=OFF
                    -DENABLE_IDRISI_VECTOR=OFF
                    -DENABLE_JML=OFF
                    -DENABLE_NTF=OFF
                    -DENABLE_ODS=OFF
                    -DENABLE_OPENAIR=OFF
                    -DENABLE_OPENFILEGDB=OFF
                    -DENABLE_OSM=OFF
                    -DENABLE_PDS_VECTOR=OFF
                    -DENABLE_PG=OFF
                    -DENABLE_PGDUMP=OFF
                    -DENABLE_REC=OFF
                    -DENABLE_S57=OFF
                    -DENABLE_SDTS_VECTOR=OFF
                    -DENABLE_SEGUKOOA=OFF
                    -DENABLE_SEGY=OFF
                    -DENABLE_SELAFIN=OFF
                    #    -DENABLE_SHAPE=OFF
                    -DENABLE_SQLITE_GPKG=OFF
                    -DENABLE_SUA=OFF
                    -DENABLE_SVG=OFF
                    -DENABLE_SXF=OFF
                    -DENABLE_TIGER=OFF
                    -DENABLE_VDV=OFF
                    -DENABLE_VFK=OFF
                    -DENABLE_WASP=OFF
                    -DENABLE_WFS=OFF
                    -DENABLE_XLSX=OFF)
