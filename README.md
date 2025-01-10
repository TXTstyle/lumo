# osen-raytracer

An OpenGl-based Raytracer, with Disney's principled BRDF diffuse.

![preview](./preview.png)

## Features

- OBJ scene loading
- Material system:
    - Diffuse maps
    - Roughness maps
    - Normal maps
- HDRI lighting

## Build

```bash
git clone https://github.com/IndaPlus24/osen-raytracer.git
cd osen-raytracer
make
```

### Dependencies

- `mold`
- `clang`
- `GLFW3`
- `GLM`
- `OpenEXR`
    - `Imath`

## Run

```bash
./bin/main
```

# Previews

## HDRI

![hdri-preview](./hdri-preview.png)
