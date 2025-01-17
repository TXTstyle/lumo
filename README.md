# Lumo

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
git clone git@github.com:TXTstyle/lumo.git
cd lumo
make
```

### Dependencies

- `CMake`
- `GLFW3`
- `GLM`
- `OpenEXR`
    - `Imath`

<details>
<summary>Arch</summary>

```bash
sudo pacman -S cmake glfw glm openexr imath
```

</details>

## Run

```bash
./build/bin/Lumo
```

Or 

```bash
make run
```

# Previews

## HDRI

![hdri-preview](./hdri-preview.png)

## Materials

![materials-preview](./materials-preview.png)
