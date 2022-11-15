
fractal_C2: fractal_C2.c
	gcc -fopenmp fractal_C2.c -o fractal_C2
	./fractal_C2 3840 2160
	rm -f out_julia.mp4
	rm -f out_mandel.mp4
	ffmpeg -framerate 5 -pattern_type glob -i "./images/julia-*.ppm" -c:v libx264 -r 30 -pix_fmt yuv420p out_julia.mp4
	ffmpeg -framerate 5 -pattern_type glob -i "./images/mandel-*.ppm" -c:v libx264 -r 30 -pix_fmt yuv420p out_mandel.mp4