#version 450
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

void main()
{
    barrier();
}

