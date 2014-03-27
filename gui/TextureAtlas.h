// #ifndef R64FX_GUI_TEXTURE_ATLAS_H
// #define R64FX_GUI_TEXTURE_ATLAS_H
// 
// #include "Texture.h"
// #include "geometry.h"
// 
// namespace r64fx{
// 
//     
// class TextureAtlas : public Texture{
//     /* Splitting the parent texure in a tree like manner. */
//     struct Node;
//     Node* root_node;
//     
// public:
//     TextureAtlas(int width, int height, int channel_count, int mode);
//     
//     struct Subtexture{
//         friend class TextureAtlas;
//         
//         inline bool isGood() const { return node != nullptr; }
//         
//         /** @brief Mark this subtexture as not taken. */
//         void free();
// 
//         Rect<float> rect();
//         
//         TextureAtlas* hostAtlas() const { return parent_atlas; }
//         
//     private:
//         TextureAtlas* parent_atlas;
//         TextureAtlas::Node* node;
//     };
//     
//     /** @brief Allocate a subtexture in the atlas. 
//      
//         Use Subtexture::isGood() to check if allocation was successful.
//      */
//     Subtexture getSubtexture(float width, float height);
//     
//     inline Subtexture getSubtexture(Size<float> size) { return getSubtexture(size.w, size.h); }
//     
//     /** @brief Full cleanup.*/
//     void free();
//     
// #ifdef DEBUG
//     void debugRenderRects();
// #endif//DEBUG
// }; 
//     
// }//namespace r64fx
// 
// #endif//R64FX_GUI_TEXTURE_ATLAS_H