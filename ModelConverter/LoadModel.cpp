#include <iostream>
#include <fstream>
#include <vector>

struct SavedVertexData {
    float pos[3];
    float uv[2];
    float norm[3];
    float boneID[4];
    float boneWeight[4];
};

struct VertexData {
    float pos[3];
    float uv[2];
    float norm[3];
};

struct IndiceData {
    unsigned int data[3];
};

int main()
{
    std::ifstream file("bigcity.bmodel", std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open input file." << std::endl;
        return -1;
    }

    std::ofstream logFile("loadedData.txt");

    while (file.peek() != EOF) {
        // Lesen der Vertex-Daten
        unsigned int vertexCount;
        file.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));

        std::vector<VertexData> vtxDat(vertexCount);
        file.read(reinterpret_cast<char*>(vtxDat.data()), vertexCount * sizeof(VertexData));

        // Ausgabe der gelesenen Vertex-Daten
        logFile << "Vertex Count: " << vertexCount << std::endl;
        for (const auto& vtx : vtxDat) {
            logFile << "Position: (" << vtx.pos[0] << ", " << vtx.pos[1] << ", " << vtx.pos[2] << ") ";
            logFile << "UV: (" << vtx.uv[0] << ", " << vtx.uv[1] << ") ";
            logFile << "Normal: (" << vtx.norm[0] << ", " << vtx.norm[1] << ", " << vtx.norm[2] << ")" << std::endl;
        }

        // Lesen der Indice-Daten
        unsigned int indexCount;
        file.read(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));

        std::vector<IndiceData> indDat(indexCount);
        file.read(reinterpret_cast<char*>(indDat.data()), indexCount * sizeof(IndiceData));

        // Ausgabe der gelesenen Indice-Daten
        logFile << "Index Count: " << indexCount << std::endl;
        for (const auto& ind : indDat) {
            logFile << "Indices: (" << ind.data[0] << ", " << ind.data[1] << ", " << ind.data[2] << ")" << std::endl;
        }

        // Lesen der Textur-Daten
        int x, y, comp;
        file.read(reinterpret_cast<char*>(&x), sizeof(x));
        file.read(reinterpret_cast<char*>(&y), sizeof(y));

        std::vector<unsigned char> texture(x * y * 4);
        file.read(reinterpret_cast<char*>(texture.data()), x * y * 4);

        // Ausgabe der Textur-Daten
        logFile << "Texture Dimensions: " << x << "x" << y << " Components: " << 4 << std::endl;

        std::vector<SavedVertexData> vertexData(vtxDat.size());
        for (auto& I : vtxDat)
        {
            vertexData.push_back({
                { I.pos[0], I.pos[1], I.pos[2] },
                { I.uv[0], I.uv[1] },
                { I.norm[0], I.norm[1], I.norm[2] },
                });
        }
    }

    return 0;
}
