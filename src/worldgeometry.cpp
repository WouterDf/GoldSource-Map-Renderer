#include "worldgeometry.h"
#include "bsp.h"
#include "bsprenderbatch.h"

WorldGeometry::WorldGeometry(BSPRenderer *renderer)
     : m_renderer( renderer )
{}

// Register BSP geometry to renderer and create RenderBatches
void WorldGeometry::Load(const BSP::BSP& bsp)
{
     const uint8_t ATTRIBUTES_PER_VERTEX = 5;
     std::vector<std::vector<float>> localVertexBuffer{};
     std::vector<std::vector<uint32_t>> localIndexBuffer{};
     std::vector<std::string> textureNames;

    // Populate the vertex and index buffers for the static level
     // geometry, face per face.
     for( const auto& face : bsp.GetFaces() )
     {
          BSP::Textureinfo textureInfo = bsp.GetTextureinfos()[face.iTextureInfo];
          BSP::MipTex textureMip = bsp.GetTextures()[textureInfo.iMiptex];

          if( textureInfo.nFlags == 0x01 )
          {
               // Flags texture as SPECIAL
               // TODO: Draw or not based on texture name (sky, water, trigger, ..)
               continue;
          }

          std::vector<float> faceVertexBuffer{};
          std::vector<uint32_t> faceIndexBuffer{};

          // Add vertices to the VBO
          for( const auto& vertex : bsp.GetVertices(face) )
          {
               // Position
               faceVertexBuffer.push_back( vertex.x );
               faceVertexBuffer.push_back( vertex.z );
               faceVertexBuffer.push_back( -vertex.y );

               // UV Coordinates
               auto glmPos = glm::vec3(vertex.x, vertex.y, vertex.z);
               auto glmS = glm::vec3(textureInfo.vS.x, textureInfo.vS.y, textureInfo.vS.z);
               auto glmT = glm::vec3(textureInfo.vT.x, textureInfo.vT.y, textureInfo.vT.z);

               float u = (glm::dot(glmPos, glmS) + textureInfo.fSShift) / textureMip.nWidth;
               float v = (glm::dot(glmPos, glmT) + textureInfo.fTShift) / textureMip.nHeight;

               faceVertexBuffer.push_back(u);
               faceVertexBuffer.push_back(v);
          }

          // Triangulate using triangle fan (v0, v1, v2), (v0, v2, v3), etc.
          // and add incides to local EBO.
          for ( int i = 1; i < face.nEdges - 1; i++ )
          {
               faceIndexBuffer.push_back( 0 );     // First vertex
               faceIndexBuffer.push_back( i );     // Current vertex
               faceIndexBuffer.push_back( i + 1 ); // Next vertex
          }

          localVertexBuffer.push_back(faceVertexBuffer);
          localIndexBuffer.push_back(faceIndexBuffer);
          textureNames.push_back(textureMip.szName);
        };


        std::vector<BSPDrawCall> drawCalls = this->m_renderer->RegisterDrawCalls(
               localVertexBuffer,
               localIndexBuffer,
               textureNames);

        this->m_drawCalls = drawCalls;
}

void WorldGeometry::DrawFrame()
{

     for( const auto& drawCall : m_drawCalls)
     {
          m_renderer->DrawFrame(drawCall);
     }
}


