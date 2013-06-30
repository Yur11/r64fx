/* Serialization/Deserialization routines for r64fx classes. */
namespace r64fx{

/** @brief Error thrown by serialization/deserialization routines.*/
class SerializationError{
    string _message;

public:
    SerializationError(string message) : _message(message) {}

    inline string message() const { return _message; }
};


JsonValue serialize(Point<float> point)
{
    auto obj = new JsonObject;
    (*obj)["x"] = point.x;
    (*obj)["y"] = point.y;
    return obj;
}


Point<float> deserialize_point_float(JsonValue value)
{
    if(!value.isObject())
    {
        throw SerializationError("deserialize_point_float: Expecting object!");
    }

    auto obj = value.asObject();

    return Point<float>(
        Json::readNumber((*obj)["x"], 0.0),
        Json::readNumber((*obj)["y"], 0.0)
    );
}


JsonValue serialize(Size<float> size)
{
    auto obj = new JsonObject;
    (*obj)["w"] = size.w;
    (*obj)["h"] = size.h;
    return obj;
}


Size<float> deserialize_size_float(JsonValue value)
{
    if(!value.isObject())
    {
        throw SerializationError("deserialize_size_float: Expecting object!");
    }

    auto obj = value.asObject();

    return Size<float>(
        Json::readNumber((*obj)["w"], 10.0),
        Json::readNumber((*obj)["h"], 10.0)
    );
}


JsonValue serialize(Widget* widget)
{
    auto obj = new JsonObject;
    (*obj)["position"] = serialize(widget->position());
    (*obj)["size"] = serialize(widget->size());
    return obj;
}


void deserialize_widget(JsonValue value, Widget* &widget)
{
    if(!value.isObject())
    {
        throw SerializationError("deserialize_widget: Expecting object!");
    }

    auto obj = value.asObject();

    widget->setPosition(deserialize_point_float(
        Json::readObject((*obj)["position"])
    ));

    widget->resize(deserialize_size_float(
        Json::readObject((*obj)["size"])
    ));
}

}//namespace r64fx