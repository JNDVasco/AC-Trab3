# AC-Trab3


Recriar pastas
`rm -dfr ex2/ && mkdir ex2`

Video da Julia  
`ffmpeg -framerate 25 -pattern_type glob -i "./ex2/julia-*.ppm" -c:v libx264 -r 30 -pix_fmt yuv420p out_julia.mp4`

Video do Manel
`ffmpeg -framerate 25 -pattern_type glob -i "./ex2/mandel-*.ppm" -c:v libx264 -r 30 -pix_fmt yuv420p out_mandel.mp4`
