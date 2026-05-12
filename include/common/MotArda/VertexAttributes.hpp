#pragma once

namespace MTRD {

    class VertexAttribute {
    public:
        //< Name of the vertex attribute
        const char* name;
        //< Size of the vertex attribute
        int size;
        //< Offset of the vertex attribute
        size_t offset;

        int location;
    };
}