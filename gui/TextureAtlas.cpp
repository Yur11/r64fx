// #include "TextureAtlas.h"
// 
// /*
// 
//     !!! (0, 0) is at the bottom left !!! (OpenGL default)
//     
//     
//      (0, h)     top     (w, h)
//          -----------------
//          |               |
//    left  |               |  right
//          |               |
//          -----------------
//      (0, 0)    bottom   (w, 0)
// */
// 
// 
// #ifdef DEBUG
// #include <assert.h>
// #include <iostream>
// using namespace std;
// #endif//DEBUG
// 
// 
// namespace r64fx{
//     
// struct TextureAtlas::Node{
//     Rect<float> rect;
//     bool is_taken = false;
//     
//     ~Node()
//     {
//         if(hasChildren())
//         {
//             TextureAtlas::Node* child = first_child;
//             for(;;)
//             {
//                 auto next_sib = child->next_sibling;
//                 
//                 delete child;
//                 
//                 if(next_sib == nullptr)
//                     break;
// 
//                 child = next_sib;
//             }
//         }
//     }
//     
//     inline TextureAtlas::Node* snatchRect(Size<float> size) { return snatchRect(size.w, size.h); }
//     
//     TextureAtlas::Node* snatchRect(float rect_width, float rect_height)
//     {
//         cout << "snatch: " << rect_width << "x" << rect_height << "\n";
//         
//         if(this->rect.width() < rect_width || this->rect.height() < rect_height)
//         {
//             cout << "Can't fit\n";
//             return nullptr; //Can't fit the rect that large.
//         }
//         
//         if(hasChildren())
//         {
//             /* snatchRect() from one of the childen. */
//             TextureAtlas::Node* child = first_child;
//             for(;;)
//             {
//                 if(child == nullptr)
//                     break; //End of the list.
//                 
//                 
//                 if(!child->is_taken)
//                 {
//                     cout << "About to snatch: " << rect_width << "x" << rect_height << "\n";
//                     auto new_node = child->snatchRect(rect_width, rect_height);
//                     if(new_node != nullptr)
//                         return new_node;
//                 }
//                 else
//                 {
//                     cout << "Child taken\n";
//                 }
//                 
//                 child = child->next_sibling;
//             }
//             
//             cout << "Failed to find childen large enough\n";
//             return nullptr;
//         }
//         else
//         {
//             if(rect.width() == rect_width && rect.height() == rect_height)
//             {
//                 cout << "Full match";
//                 this->is_taken = true;
//                 return this;
//             }
//             
//             auto a = new TextureAtlas::Node;
//             auto b = new TextureAtlas::Node;
//             
//             if(rect.width() == rect_width)
//             {
//                 cout << "Split top bottom\n";
//                 this->splitTopBottom(rect_height, a, b);
//                 
//                 this->first_child = b;
//                 b->next_sibling = a;
//                 a->is_taken = true;
//                 
//                 return a;
//             }
//             
//             if(rect.height() == rect_height)
//             {
//                 cout << "Split left right\n";
//                 this->splitLeftRight(rect_width, a, b);
//                 
//                 this->first_child = b;
//                 b->next_sibling = a;
//                 a->is_taken = true;
//                 return a;
//             }
//             
//             /* Split into three parts. */
//             cout << "Split into 3\n";
//             splitTopBottom(rect_height, a, b);
//             
//             auto a1 = new TextureAtlas::Node;
//             auto a2 = new TextureAtlas::Node;
//             a->splitLeftRight(rect_width, a1, a2);
//             
//             this->first_child = b;
//             b->next_sibling = a2;
//             a2->next_sibling = a1;
//             
//             a1->is_taken = true;
//             return a1;
//         }
//     }
// 
//     inline bool hasNextSibling() { return next_sibling != nullptr; }
//     
//     inline bool hasChildren() { return first_child != nullptr; }
//     
//     void splitTopBottom(float rect_height, TextureAtlas::Node* &a, TextureAtlas::Node* &b)
//     {
//         a->rect.left = b->rect.left = this->rect.left;
//         a->rect.right = b->rect.right = this->rect.right;
//         
//         a->rect.bottom = this->rect.bottom;
//         a->rect.top = a->rect.bottom + rect_height;
//         b->rect.bottom = a->rect.top;
//         b->rect.top = this->rect.top;
//     }
//     
//     void splitLeftRight(float rect_width, TextureAtlas::Node* &a, TextureAtlas::Node* &b)
//     {
//         a->rect.top = b->rect.top = this->rect.top;
//         a->rect.bottom = b->rect.bottom = this->rect.bottom;
//         
//         a->rect.left = this->rect.left;
//         a->rect.right = a->rect.left + rect_width;
//         b->rect.left = a->rect.right;
//         b->rect.right = this->rect.right;
//     }
//     
// #ifdef DEBUG
//     void debugRender()
//     {
//         if(hasChildren())
//         {
//             TextureAtlas::Node* child = first_child;
//             for(;;)
//             {
//                 if(child == nullptr)
//                     break; //End of the list.
//                     
//                 child->debugRender();
//                     
//                 child = child->next_sibling;
//             }
//         }
//         else
//         {
//             if(is_taken)
//                 glColor3f(1.0, 0.0, 0.0);
//             else
//                 glColor3f(0.0, 1.0, 0.0);
//             
// //             glBegin(GL_LINE_LOOP);
// //                 glVertex2f(rect.left, rect.bottom);
// //                 glVertex2f(rect.right, rect.bottom);
// //                 glVertex2f(rect.right, rect.top);
// //                 glVertex2f(rect.left, rect.top);
// //             glEnd();
//         }
//     }
// #endif//DEBUG
//     
// private:
//     TextureAtlas::Node* next_sibling = nullptr;
//     TextureAtlas::Node* first_child = nullptr;
// };
// 
// 
// void TextureAtlas::Subtexture::free()
// {
// #ifdef DEBUG
//     assert(node != nullptr);
// #endif//DEBUG
//     node->is_taken = false;
// }
// 
// 
// Rect<float> TextureAtlas::Subtexture::rect()
// {
// #ifdef DEBUG
//     assert(node != nullptr);
// #endif//DEBUG
//     return node->rect;
// }
// 
// 
// TextureAtlas::TextureAtlas(int width , int height, int channel_count, int mode)
// : Texture(width, height, channel_count, mode)
// {
//     root_node = new TextureAtlas::Node;
//     root_node->rect = { Point<float>(0.0, 0.0), Size<float>(Texture::width(), Texture::height())};
// }
//     
// 
// TextureAtlas::Subtexture TextureAtlas::getSubtexture(float width, float height)
// {
//     TextureAtlas::Subtexture subtx;
//     subtx.node = root_node->snatchRect(width, height);
//     subtx.parent_atlas = this;
//     return subtx;
// }
// 
// 
// void TextureAtlas::free()
// {
//     Texture::free();
//     delete root_node;
// }
// 
// 
// #ifdef DEBUG
// void TextureAtlas::debugRenderRects()
// {
//     assert(root_node != nullptr);
//     root_node->debugRender();
// }
// #endif//DEBUG
//     
// }//namespace r64fx