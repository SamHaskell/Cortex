using System.Collections.Generic;
using Microsoft.Xna.Framework;

namespace Cortex;

public interface ISpatialNode {
    public ISpatialNode Parent {get;}
    public List<ISpatialNode> Children {get;}
    public Matrix LocalTransform {get; set;}
    public Matrix WorldTransform {get;}
}