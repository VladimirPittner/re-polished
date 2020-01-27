module Utils = {
  let cssToColor =
      (cssColor: Css.Types.Color.t): option(Polished.Types.color) => {
    Polished.Types.(
      switch (cssColor) {
      | `rgb(r, g, b) =>
        Some(Polished.Types.RGB(RGB.make(~red=r, ~green=g, ~blue=b)))
      | `rgba(r, g, b, a) =>
        Some(RGBA(RGBA.make(~red=r, ~green=g, ~blue=b, ~alpha=a)))
      | `hex(str) => Some(HEX(HEX.make(str)))
      | `hsl(h, s, l) =>
        let deg: float =
          switch (h) {
          | `grad(f) => f // WRONG!!!
          | `turn(f) => f // WRONG!!!
          | `deg(f) => f
          | `rad(f) => f // WRONG!!!
          };
        Some(
          HSL(
            HSL.make(
              ~hue=deg,
              ~saturation=
                switch (s) {
                | `percent(f) => f
                },
              ~lightness=
                switch (l) {
                | `percent(f) => f
                },
            ),
          ),
        );
      | _ => None
      }
    );
  };

  let colorToCss = (color: Polished.Types.color): Css.Types.Color.t => {
    Polished.Types.(
      switch (color) {
      | RGB(rgb) =>
        Css.rgb(
          RGB.red(rgb) |> Int8.asInt,
          RGB.green(rgb) |> Int8.asInt,
          RGB.blue(rgb) |> Int8.asInt,
        )
      | RGBA(rgba) =>
        Css.rgba(
          RGBA.red(rgba) |> Int8.asInt,
          RGBA.green(rgba) |> Int8.asInt,
          RGBA.blue(rgba) |> Int8.asInt,
          RGBA.alpha(rgba) |> Percent.asFloat,
        )
      | HEX(hex) => Css.hex(HEX.asString(hex))
      | HSL(hsl) =>
        Css.hsl(
          Css.Types.Angle.deg(HSL.hue(hsl) |> Degree.asFloat),
          HSL.saturation(hsl) |> Percent.asFloat,
          HSL.lightness(hsl) |> Percent.asFloat,
        )
      | HSLA(hsla) =>
        Css.hsla(
          Css.Types.Angle.deg(HSLA.hue(hsla) |> Degree.asFloat),
          HSLA.saturation(hsla) |> Percent.asFloat,
          HSLA.lightness(hsla) |> Percent.asFloat,
          `percent(HSLA.alpha(hsla) |> Percent.asFloat),
        )
      }
    );
  };
};

let transparentize =
    (percentage: float, cssColor: Css.Types.Color.t): Css.Types.Color.t => {
  let maybeColor = Utils.cssToColor(cssColor);
  let percent = Polished.Types.Percent.make(percentage);
  switch (maybeColor) {
  | Some(c) =>
    let tr = Polished.Color.Transparentize.transparentize(percent, c);
    let newColor = Utils.colorToCss(tr);
    newColor;
  | _ =>
    Js.log(
      "Transparentize failed. Given percentage or css color was invalid",
    );
    cssColor;
  };
};