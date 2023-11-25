#version 450
layout(binding = 0) buffer BufferA {
    float dataA[];
};
layout(binding = 1) buffer BufferB {
    float dataB[];
};
layout(binding = 2) buffer BufferResult {
    float dataResult[];
};

layout(local_size_x = 1) in;

void main() {
    uint id = gl_GlobalInvocationID.x;
    dataResult[id] = dataA[id] + dataB[id];
}
