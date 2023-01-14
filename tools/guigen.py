import yaml
import argparse
import os
import re


class GenerateError(Exception):
    def __init__(self, message):
        self.message = message


def boundaries(o) -> str:
    b = o["boundaries"]
    m = re.match(re.compile("(.+?),\\s?(.+?),\\s?(.+?),\\s?(.+?)"), b)
    if m:
        return "{0}, {1}, {2}, {3}".format(m.group(1), m.group(2), m.group(3), m.group(4))
    raise GenerateError("Incorrect boundaries: {0}".format(b))


def prev_base(prev) -> str:
    return "&{0}.base".format(prev["name"]) if prev else "NULL"


def button(my_type, o, prev, generate_buffer, generate_callback) -> str:
    name = o["name"]
    key = o["key"]
    icon = o["icon"]
    if "title" in o:
        title = "\"{0}\"".format(o["title"])
    elif "buffer" in o:
        title = o["buffer"]
        buffer_size = o["buffer_size"]
        generate_buffer(title, buffer_size)
    else:
        raise GenerateError("Either title or buffer/buffer_size has to be specified")
    callback = o["callback"]
    generate_callback(callback, "void", "void")
    return "static struct gui_button_t {0} = zxgui_button_init({1}, {2}, {3}, {4}, {5}, {6});".format(
        name, prev_base(prev), boundaries(o), key, icon, title, callback
    )


def label(my_type, o, prev, generate_buffer, generate_callback) -> str:
    name = o["name"]
    color = o["color"]
    flags = o["flags"]
    if "title" in o:
        title = "\"{0}\"".format(o["title"])
    elif "buffer" in o:
        title = o["buffer"]
        buffer_size = o["buffer_size"]
        generate_buffer(title, buffer_size)
    else:
        raise GenerateError("Either title or buffer/buffer_size has to be specified")
    return "static struct gui_label_t {0} = zxgui_label_init({1}, {2}, {3}, {4}, {5});".format(
        name, prev_base(prev), boundaries(o), title, color, flags
    )


def edit(my_type, o, prev, generate_buffer, generate_callback) -> str:
    name = o["name"]
    buffer = o["buffer"]
    buffer_size = o["buffer_size"]
    generate_buffer(buffer, buffer_size)
    my_type_obj = "zxgui_multiline_edit_init" if my_type == "multiline_edit" else "zxgui_edit_init"
    return "static struct gui_edit_t {0} = {1}({2}, {3}, {4}, {5});".format(
        name, my_type_obj, prev_base(prev), boundaries(o), buffer, buffer_size
    )


def select(my_type, o, prev, generate_buffer, generate_callback) -> str:
    name = o["name"]
    obtain_data_cb = o["obtain_data"]
    selected_cb = o["selected"]
    options_capacity = o["options_capacity"]
    user = o["user"]
    generate_callback(obtain_data_cb, "void", "uint8_t*")
    generate_callback(selected_cb, "struct gui_select_option_t* selected", "void")
    return "static struct gui_select_t {0} = zxgui_select_init({1}, {2}, {3}, {4}, {5}, {6});".format(
        name, prev_base(prev), boundaries(o), obtain_data_cb, options_capacity, user, selected_cb
    )


def image(my_type, o, prev, generate_buffer, generate_callback) -> str:
    name = o["name"]
    if "generate" in o:
        generate_image = o["generate"]
        script_directory = os.path.dirname(os.path.realpath(__file__))
        script = os.path.join(script_directory, "png2c.py")
        id_name = name + "_generated_img"
        header_name = name + ".img.h"
        call_s = "python3 {0} {1} --no-print-string --id={2} --dump-color --preferred-bg=black > {3}".format(
            script, generate_image, id_name, header_name
        )
        print("generating an image: {0}".format(call_s))
        if os.system(call_s):
            raise GenerateError("Cannot generate {0} from {1}".format(header_name, generate_image))
        source = id_name
        colors = id_name + "_colors"
        return "#include \"{0}\"\nstatic struct gui_image_t {1} = zxgui_image_init({2}, {3}, {4}, {5});".format(
            header_name, name, prev_base(prev), boundaries(o), source, colors)
    else:
        source = o["source"]
        colors = o["colors"]
    return "static struct gui_image_t {0} = zxgui_image_init({1}, {2}, {3}, {4});".format(
        name, prev_base(prev), boundaries(o), source, colors
    )


SUPPORTED_TYPES = {
    "button": button,
    "label": label,
    "edit": edit,
    "multiline_edit": edit,
    "select": select,
    "image": image,
}


def generate(source, target):
    try:
        with open(source, "r") as f:
            document = yaml.load(f, yaml.Loader)
    except yaml.YAMLError as e:
        print("Could not parse source {0}: {1}".format(source, str(e)))
        exit(1)

    print("{0} -> {1}".format(source, target))

    if "version" not in document:
        print("... missing version")
        return False

    if document["version"] != 1:
        print("... unknown version: {0}".format(document["version"]))
        return False

    if "scenes" not in document:
        print("... missing scenes")
        return False

    with open(target, "w") as f:
        f.write("#include \"zxgui.h\"\n")
        f.write("#include <stdint.h>\n")
        f.write("// AUTO-GENERATED FILE FROM {0}\n\n".format(source))

        scene_objects_map = {}
        buffers = {}
        callbacks = {}

        for scene_name, scene in document["scenes"].items():
            if scene is None:
                print("error on scene {0}: missing objects".format(scene_name))
                return False

            f.write("/* ================ SCENE {0} ================ */\n\n".format(scene_name))

            def err_object(i, reason):
                o = scene[i]
                if "name" in o:
                    print("error on scene {0} object {1} at index {2}: {3}".format(scene_name, o["name"], i, reason))
                else:
                    print("error on scene {0} object {1}: {2}".format(scene_name, i, reason))
                return False

            scene_objects_list = []
            local_buffers = {}
            local_callbacks = {}
            previous_object = None
            focus = "NULL"

            def generate_buffer(buffer_name, buffer_size):
                if buffer_name in buffers:
                    raise GenerateError("Buffer {0} already exists".format(buffer_name))
                buffers[buffer_name] = int(buffer_size)
                local_buffers[buffer_name] = buffers[buffer_name]

            def generate_callback(callback_name, arguments, result):
                if callback_name in callbacks:
                    raise GenerateError("Callback {0} already exists".format(callback_name))
                callbacks[callback_name] = (arguments, result)
                local_callbacks[callback_name] = callbacks[callback_name]

            for index, o in reversed(list(enumerate(scene))):
                if "name" not in o:
                    return err_object(index, "missing name field")
                name = o["name"]
                if name in scene_objects_map:
                    return err_object(index, "object with name {0} already exists".format(name))
                if "type" not in o:
                    return err_object(index, "missing type field")
                tp = o["type"]
                if tp not in SUPPORTED_TYPES:
                    return err_object(index, "unsupported type: {0}".format(tp))
                if "focus" in o and bool(o["focus"]):
                    focus = prev_base(o)
                try:
                    data = SUPPORTED_TYPES[tp](tp, o, previous_object, generate_buffer, generate_callback)
                except KeyError as e:
                    return err_object(index, "missing key: {0}".format(str(e)))
                except GenerateError as e:
                    return err_object(index, "{0}".format(str(e.message)))

                generated_object = {
                    "name": name,
                    "data": data
                }

                scene_objects_map[name] = generated_object
                scene_objects_list.append(generated_object)

                previous_object = o

            if len(local_callbacks):
                f.write("// CALLBACKS:\n")
                for b, s in local_callbacks.items():
                    arguments, returns = s
                    f.write("static {0} {1}({2});\n".format(returns, b, arguments))
            if len(local_buffers):
                f.write("// BUFFERS:\n")
                for b, s in local_buffers.items():
                    f.write("char {0}[{1}];\n".format(b, s))
            f.write("// COMPONENTS:\n")
            for o in scene_objects_list:
                f.write("{0}\n".format(o["data"]))
            f.write("// SCENE:\n")
            f.write("static struct gui_scene_t " + scene_name + " = {" + prev_base(previous_object) + ", " +
                    focus + "};\n\n")

    print("Done.")
    return True


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog='guigen.py',
        description='Generate zxgui C file from yaml source')

    parser.add_argument('-s', '--source', help="Source YAML file or folder (in that case suffix would be required)")
    parser.add_argument('-t', '--target', help="A C file to generate from")
    parser.add_argument('-x', '--suffix', help="A filename suffix for output in case target is omitted")

    args = parser.parse_args()

    if args.target is None and args.suffix is None:
        print("Either --target or --suffix has to be provided")
        exit(2)

    if os.path.isdir(args.source):
        if args.suffix is None:
            print("--suffix has to be provided")
            exit(3)

        for file_name in os.listdir(args.source):
            if not file_name.endswith(".yaml"):
                continue
            f = os.path.join(args.source, file_name)
            t = os.path.join(args.source, file_name[:-5] + "." + args.suffix)

            if not generate(f, t):
                exit(4)
    else:
        if not generate(args.source, args.target):
            exit(5)
