using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RBCubeMovement : MonoBehaviour
{
    public float torque;
    private Rigidbody rb;

    // Start is called before the first frame update
    void Start()
    {
        rb = GetComponent<Rigidbody>();

    }

    // Update is called once per frame
    void Update()
    {

        float turn = Input.GetAxis("Vertical");
        rb.AddTorque(Vector3.right * torque * turn);

        float hTurn = Input.GetAxis("Horizontal");
        rb.AddTorque(Vector3.forward * torque * -hTurn);

        /*if (Input.GetKey(KeyCode.UpArrow))
        {
            float turn = Input.GetAxis("Vertical");
            rb.AddTorque(Vector3.right * torque * turn);
        }
        if (Input.GetKey(KeyCode.DownArrow))
        {
            float turn = Input.GetAxis("Vertical");
            rb.AddTorque(Vector3.right * torque * turn);
        }
        if (Input.GetKey(KeyCode.RightArrow))
        {
            float turn = Input.GetAxis("Horizontal");
            rb.AddTorque(Vector3.forward * torque * turn);
        }
        if (Input.GetKey(KeyCode.LeftArrow))
        {
            float turn = Input.GetAxis("Horizontal");
            rb.AddTorque(Vector3.forward * torque * turn);
        }*/

        //float turn = Input.GetAxis("Horizontal");
        //rb.AddTorque(transform.forward * torque * turn);
    }
}
