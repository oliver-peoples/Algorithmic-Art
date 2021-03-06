#include <iostream>
#include <string>
#include <vector>
#include <hmath.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <stdlib.h>
#include <time.h>

double randDouble()
{
    return (double(rand()) - double(RAND_MAX / 2)) / double(RAND_MAX / 2);
}

struct Particle
{
    bool simulated;
    hmath::Vector3 position;
};

int main()
{
    // You can adjust these

    cv::Size aspect_ratio = { 16,9 };
    double grid_subsampling = 2;
    int top = 1024;
    size_t t_duration = 20;
    double t_step_size = 0.01;
    size_t particles = 5000;
    size_t iteration_limit = 300;
    double scalar = 0.1;
    int line_thickness = 1;
    cv::Scalar background = { 20,0,0 };
    cv::Scalar line_color = { 255,255,255 };
    bool blur = false;
    bool record = false;
    bool normed = false;
    bool display_vectors = true;

    cv::Size cells = { 16,9 };

    // Don't touch anything below here unless you really know
    // what you are doing, just read!

    srand(time(NULL));

    cv::Size img_size = { top,int(ceil(double(top) * (double(aspect_ratio.height) / double(aspect_ratio.width))))};
    cv::Size points = { cells.width + 1,cells.height + 1 };
    double cell_edge = grid_subsampling;
    hmath::Vector2 grid_spatial_dimensions = { double(cells.width) * grid_subsampling,double(cells.height) * grid_subsampling };
    hmath::Vector2 grid_img_map = { double(img_size.width) / grid_spatial_dimensions.i,double(img_size.height) / grid_spatial_dimensions.j };
    
    cv::VideoWriter vector_field_art;

    if (record)
    {
        vector_field_art.open("../vector_field_art.mp4", cv::VideoWriter::fourcc('m','p','4','v'), 60, img_size, true);
    }
    
    std::cout << "\nRunning random spatiotemporal vector field simulation! Here are the simulation parameters:\n" << std::endl;
    std::cout << " > Image aspect ratio: " << aspect_ratio << std::endl;
    std::cout << " > Image dimensions: " << img_size << std::endl;
    std::cout << " > Grid count: " << cells << std::endl;
    std::cout << " > Grid spatial dimensions: " << grid_spatial_dimensions << std::endl;
    std::cout << " > Point count: " << points << std::endl;
    std::cout << " > Mapping form grid to image: " << grid_img_map << std::endl;
    std::cout << " > Frame timestep: " << t_step_size << "\n" << std::endl;

    // For this field, we'll use a [t,i,j] structure

    hmath::Vector2Field3d vector_field(grid_subsampling);

    for (size_t t = 0; t <= t_duration; t++)
    {
        std::vector<std::vector<hmath::Vector2>> columns;

        for (size_t i = 0; i <= cells.width; i++)
        {
            std::vector<hmath::Vector2> column;

            for (size_t j = 0; j <= cells.height; j++)
            {
                if (normed)
                {
                    column.push_back(hmath::Vector2(randDouble(),randDouble()).normed());
                }
                
                else
                {
                    column.push_back((hmath::Vector2(randDouble(),randDouble()) * abs(randDouble())));
                }
                
            }

            columns.push_back(column);
        }

        vector_field.getField().push_back(columns);
    }

    std::vector<Particle> master_particles;

    for (size_t particle_n = 0; particle_n < particles; particle_n++)
    {
        master_particles.push_back({ true,{ 0,abs(randDouble()) * grid_spatial_dimensions.i,abs(randDouble()) * grid_spatial_dimensions.j } });
    }

    int frame_n = 0;
    int n_frames = t_duration / t_step_size;

    for (double t_step = 0; t_step < (t_duration * grid_subsampling); t_step += (t_step_size * grid_subsampling))
    {
        std::cout << "Progress: " << int((double(frame_n) / double(n_frames)) * 100) << "%\r";
        std::cout.flush();

        std::vector<Particle> t_step_particles = master_particles;

        size_t remaining_particles = t_step_particles.size();

        size_t iteration = 0;

        cv::Mat frame(img_size, CV_8UC3, background);

        while (remaining_particles > 0 && iteration < iteration_limit)
        {
            for (size_t particle_n = 0; particle_n < t_step_particles.size(); particle_n++)
            {
                if (t_step_particles[particle_n].simulated == true)
                {
                    hmath::Vector3 old_position = t_step_particles[particle_n].position;
                    hmath::Vector2 subsampled = vector_field.subsample(t_step_particles[particle_n].position);
                    hmath::Vector3 subsampled_propagator = { 0,subsampled.i,subsampled.j };
                    t_step_particles[particle_n].position += (subsampled_propagator * scalar);

                    if (vector_field.inField(t_step_particles[particle_n].position))
                    {
                        cv::line(frame, { old_position.j * grid_img_map.i,old_position.k * grid_img_map.j }, { t_step_particles[particle_n].position.j * grid_img_map.i,t_step_particles[particle_n].position.k * grid_img_map.j }, line_color, line_thickness);
                    }
                    
                    else
                    {
                        t_step_particles[particle_n].simulated = false;
                        remaining_particles --;
                    }
                }
            }

            iteration ++;
        }

        for (Particle& particle : master_particles)
        {
            particle.position.i += t_step_size;
        }

        if (blur)
        {
            cv::GaussianBlur(frame, frame, { 5,5 }, 30, 30);
        }

		if (display_vectors)
		{
            for (double j = 0; j < points.height; j++)
            {
                for (double i = 0; i < points.width; i++)
                {
                    cv::Point local_origin = { i * cell_edge * grid_img_map.i,j * cell_edge * grid_img_map.j };

                    hmath::Vector2 t0_vector = vector_field.getField()[int(floor(t_step))][int(i)][int(j)];
                    hmath::Vector2 t1_vector = vector_field.getField()[int(ceil(t_step))][int(i)][int(j)];

                    hmath::Vector2 lerped_vector = t0_vector + (t1_vector - t0_vector) * (2 * t_step);
                    cv::arrowedLine(frame, { local_origin.x, local_origin.y }, { local_origin.x + lerped_vector.i * grid_img_map.i,local_origin.y + lerped_vector.j * grid_img_map.j }, { 255,0,255 }, 2);
                }
                
            }
		}
        
        vector_field_art.write(frame);

        cv::imshow("Trace", frame);
        cv::waitKey(10);

        frame_n ++;
    }

    if (record)
    {
        vector_field_art.release();
    }
    
    return 0;  
}