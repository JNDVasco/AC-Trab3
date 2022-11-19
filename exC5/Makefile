
fractal: fractal_C5.c
	gcc -fopenmp fractal_C5.c -o fractal_C5
	rm -rf images && mkdir -p ./images
	./fractal_C5 3840 2160 100 0.2
	rm -f out_julia_diff.mp4
	rm -f out_mandel_diff.mp4
	ffmpeg -framerate 10 -pattern_type glob -i "./images/Julia_diff-*.ppm" -c:v libx264 -r 30 -pix_fmt yuv420p out_julia_diff.mp4
	ffmpeg -framerate 10 -pattern_type glob -i "./images/Mandel_diff-*.ppm" -c:v libx264 -r 30 -pix_fmt yuv420p out_mandel_diff.mp4

clean: 
	rm -rf images 
	rm -f out_julia_diff.mp4
	rm -f out_mandel_diff.mp4
	rm -f mandel.pgm julia.pgm fractal_C5
