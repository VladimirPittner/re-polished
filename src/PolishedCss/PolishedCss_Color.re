module Utils = {
  let cssPercentToPercent =
      (percent: [ | `percent(float)]): Polished.Types.Percent.t =>
    switch (percent) {
    | `percent(f) => Polished.Types.Percent.make(f)
    };

  let cssAngleToDegree = (angle: Css.Types.Angle.t): Polished.Types.Degree.t =>
    Polished.Types.(
      switch (angle) {
      | `grad(f) => Degree.make(f *. 0.9)
      | `turn(f) => Degree.make(f *. 360.0 /. pi)
      | `deg(f) => Degree.make(f)
      | `rad(f) => Degree.make(f *. 180.0 /. pi)
      }
    );

  let cssToColor =
      (cssColor: Css.Types.Color.t): option(Polished.Types.color) => {
    Polished.Types.(
      switch (cssColor) {
      | `rgb(r, g, b) => Some(RGB(RGB.fromPrimitives(r, g, b)))
      | `rgba(r, g, b, a) => Some(RGBA(RGBA.fromPrimitives(r, g, b, a)))
      | `hex(str) => Some(HEX(HEX.make(str)))
      | `hsl(h, s, l) =>
        Some(
          HSL(
            HSL.make(
              ~hue=cssAngleToDegree(h),
              ~saturation=cssPercentToPercent(s),
              ~lightness=cssPercentToPercent(l),
            ),
          ),
        )
      | `hsla(h, s, l, a) =>
        Some(
          HSLA(
            HSLA.make(
              ~hue=cssAngleToDegree(h),
              ~saturation=cssPercentToPercent(s),
              ~lightness=cssPercentToPercent(l),
              ~alpha=
                switch (a) {
                | `num(f) => Percent.make(f)
                | `percent(f) => Percent.make(f)
                },
            ),
          ),
        )
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
}; // Utils

let transparentize =
    (cssColor: Css.Types.Color.t, percentage: float): Css.Types.Color.t => {
  let maybeColor = Utils.cssToColor(cssColor);
  let percent = Polished.Types.Percent.make(percentage);
  switch (maybeColor) {
  | Some(c) =>
    let tr = c->Polished.Color.transparentize(percent);
    let newColor = Utils.colorToCss(tr);
    newColor;
  | _ =>
    Js.log(
      "Transparentize failed. Given percentage or css color was invalid",
    );
    cssColor;
  };
};

let readable =
    (
      cssColor: Css.Types.Color.t,
      ~onLight: option(Css.Types.Color.t)=?,
      ~onDark: option(Css.Types.Color.t)=?,
      (),
    )
    : Css.Types.Color.t => {
  switch (Utils.cssToColor(cssColor)) {
  | Some(color) =>
    let maybeLight =
      switch (onLight) {
      | Some(l) => Utils.cssToColor(l)
      | None => None
      };
    let maybeDark =
      switch (onDark) {
      | Some(d) => Utils.cssToColor(d)
      | None => None
      };
    let read =
      Polished.Color.readable(
        color,
        ~onLight=?maybeLight,
        ~onDark=?maybeDark,
        (),
      );
    Utils.colorToCss(read);
  | None =>
    Js.log("Readable failed. Given css color(s) was invalid");
    cssColor;
  };
};

let opacify = (cssColor: Css.Types.Color.t, amount: float): Css.Types.Color.t => {
  switch (Utils.cssToColor(cssColor)) {
  | Some(color) =>
    let value =
      Polished.Color.opacify(color, Polished.Types.Percent.make(amount));
    Utils.colorToCss(value);
  | None =>
    Js.log("Opacify failed. Given css color(s) was invalid");
    cssColor;
  };
};

let darken = (cssColor: Css.Types.Color.t, amount: float): Css.Types.Color.t => {
  switch (Utils.cssToColor(cssColor)) {
  | Some(color) =>
    let value =
      Polished.Color.darken(color, Polished.Types.Percent.make(amount));
    Utils.colorToCss(value);
  | None =>
    Js.log("Darken failed. Given css color(s) was invalid");
    cssColor;
  };
};