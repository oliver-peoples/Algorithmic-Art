double grid_scale = 1;
    double img_scale = 100;

    cv::Size image_resolution = { aspect_ratio[0] * img_scale,aspect_ratio[1] * img_scale };
    cv::Size grid_resolution = { aspect_ratio[0] * grid_scale,aspect_ratio[1] * grid_scale };

    double slowness = 30;

    while (true)
    {
        size_t iterations = 300;

        hmath::VectorField2d perlin_field;

        std::vector<cv::Mat> past_imgs;

        for (size_t j = 0; j <= grid_resolution.height; j++)
        {
            std::vector<hmath::Vector2> row;

            for (size_t i = 0; i <= grid_resolution.width; i++)
            {
                row.push_back({ abs(randDouble()) * 0.5,randDouble() });
            }

            perlin_field.getField().push_back(row);
        }

        std::vector<hmath::Vector2> locations;

        size_t n_particles = 400;
        
        for (size_t n = 0; n < n_particles; n++)
        {
            hmath::Vector2 location = { abs(randDouble()) * double(grid_resolution.width),abs(randDouble()) * double(grid_resolution.height) };
            locations.push_back(location);
        }
        
        cv::Mat trace(image_resolution, CV_8UC4, { 0,0,0,0 });
        
        hmath::Vector2 old_location;

        while (n_particles > 0 && iterations > 0)
        {
            for (hmath::Vector2& location : locations)
            
            {
                if (perlin_field.inField(location))
                {
                    old_location = location;
                    location += (perlin_field.subsample(location) * (1/slowness));
                    cv::line(trace, { old_location.i * (img_scale / grid_scale),old_location.j * (img_scale / grid_scale) }, { location.i * (img_scale / grid_scale),location.j * (img_scale / grid_scale) }, { 255,0,0,0 }, 2);
                    
                }

                else
                {
                    n_particles--;
                }
            }

            cv::imshow("Trace", trace);
            cv::waitKey(10);
            cv::blur(trace, trace, { 5,5 });
            iterations--;
        }
    }