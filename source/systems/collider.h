#pragma once

vec quad_center(const quad q)
{
    vec result =
    {
        (f32)(q.tl.x + q.tr.x + q.bl.x + q.br.x) / 4.f,
        (f32)(q.tl.y + q.tr.y + q.bl.y + q.br.y) / 4.f
    };

    return result;
}

quad calculate_bounds(vec pos, f32 rotation, vec scale, size s)
{
    quad result;
    size a = s;

    f32 angle = to_radians(rotation);

    a = f2_mul(a, scale);

    // CW order
    result.tl = (vec){ pos.x - a.w / 2, pos.y - a.h / 2 };
    result.tr = (vec){ pos.x + a.w / 2, pos.y - a.h / 2 };  
    result.br = (vec){ pos.x + a.w / 2, pos.y + a.h / 2 };
    result.bl = (vec){ pos.x - a.w / 2, pos.y + a.h / 2 };

    if (angle != 0)
    {
        rotate(&result.tl, pos, angle);
        rotate(&result.tr, pos, angle);
        rotate(&result.br, pos, angle);
        rotate(&result.bl, pos, angle);
    }

    return result;
}

quad model_bounds(const vec3 pos, const vec3 scale, const vec3 s, const f32 rot)
{
    return calculate_bounds(
        (vec){ pos.x, pos.z },
        -rot,
        (vec) { scale.x, scale.z },
        (size){ s.x, s.z }
        );
}

collider collider_from_quad(const quad q)
{
    collider c = { 0 };

    // this order is super important
    c.points[0] = q.tl;
    c.points[1] = q.bl;
    c.points[2] = q.br;
    c.points[3] = q.tr;

    // build edges
    vec p1;
    vec p2; 

    for (u8 i = 0; i < COLLIDER_POINT_SIZE; i++)
    {
        p1 = c.points[i];

        if (i + 1 >= COLLIDER_POINT_SIZE)
            p2 = c.points[0];
        else
            p2 = c.points[i + 1];

        p2 = f2_sub(p2, p1);        
        c.edges[i] = p2;
    }   

    // center
    c.center = quad_center(q);

    return c;
}

min_max project_collider(vec axis, collider* c)
{
    min_max result = { 0 };

    // To project a point on an axis use the dot product
    f32 d = f2_product(axis, c->points[0]);

    result.min = d;
    result.max = d;

    for (u8 i = 0; i < COLLIDER_POINT_SIZE; i++)
    {
        d = f2_product(axis, c->points[i]);

        if (d < result.min)
            result.min = d;
        else if (d > result.max)
            result.max = d;
    }

    return result;
}

f32 interval_distance_calc(min_max pA, min_max pB)
{
    if (pA.min < pB.min)
        return pB.min - pA.max;
    else
        return pA.min - pB.max;
}

collision collision_test(collider* polygonA, collider* polygonB, const vec velocity)
{
    collision result = { 0 };

    result.min = F2_ZERO;

    result.intersect = true;
    result.will_intersect = true;

    f32 minIntervalDistance = 65535.f;

    vec translationAxis = F2_ZERO;
    vec edge;

    for (u8 index = 0; index < COLLIDER_POINT_SIZE * 2; index++)
    {
        if (index < COLLIDER_POINT_SIZE)
            edge = polygonA->edges[index];
        else
            edge = polygonB->edges[index - COLLIDER_POINT_SIZE];

        vec axis = { -edge.y, edge.x };
        axis = f2_normalize(axis);

        min_max pA = project_collider(axis, polygonA);
        min_max pB = project_collider(axis, polygonB);

        if (interval_distance_calc(pA, pB) > 0) result.intersect = false;

        f32 velocityProjection = f2_product(axis, velocity);

        if (velocityProjection < 0)
            pA.min += velocityProjection;
        else
            pA.max += velocityProjection;

        f32 intervalDistance = interval_distance_calc(pA, pB);
        if (intervalDistance > 0) result.will_intersect = false;

        if (!result.intersect && !result.will_intersect) break;

        intervalDistance = fabs(intervalDistance);

        if (intervalDistance < minIntervalDistance)
        {
            minIntervalDistance = intervalDistance;
            translationAxis = axis;

            vec d = f2_sub(polygonA->center, polygonB->center);

            if (f2_product(d, translationAxis) < 0)
            {
                translationAxis.x *= -1.f;
                translationAxis.y *= -1.f;
            }
        }
    }

    if (result.will_intersect)
        result.min = (vec)
        {
            translationAxis.x * minIntervalDistance,
            translationAxis.y * minIntervalDistance
        };

    result.intersect = result.intersect && 
        ! f2_equals(result.min, F2_ZERO);

    result.will_intersect = result.will_intersect && 
        ! f2_equals(result.min, F2_ZERO);

    return result;
}