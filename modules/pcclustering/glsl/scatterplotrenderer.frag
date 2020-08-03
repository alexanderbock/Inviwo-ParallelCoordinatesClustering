in flat int identifier;
in flat int nClusters;

uniform int _nData;
uniform bool _hasColoringData;
uniform sampler2D _transFunc;
uniform bool _excludeZero;

void main() {
    if (_excludeZero && identifier == 0) {
        discard;
    }
    
    float d = 1.0 - length(gl_PointCoord - vec2(0.5));
    float alpha = clamp(d * 1.0, 0.0, 1.0);

    if (_hasColoringData) {
        vec4 color = vec4(texture(_transFunc, vec2(float(identifier) / float(nClusters), 0.5)).rgb, alpha);
        FragData0 = color;
    }
    else {
        FragData0 = vec4(1.0, 1.0, 1.0, alpha);
    }
}
