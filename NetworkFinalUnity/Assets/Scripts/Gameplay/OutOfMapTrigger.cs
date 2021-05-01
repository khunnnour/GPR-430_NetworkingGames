using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class OutOfMapTrigger : MonoBehaviour
{
    private Vector3 spawn;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    private void OnTriggerEnter(Collider other) 
    {
        spawn.x = 0.0f;
        spawn.y = 1.0f;
        spawn.z = 0.0f;
        other.GetComponent<Rigidbody>().velocity = new Vector3(0.0f, 0.0f, 0.0f);
        other.GetComponent<Rigidbody>().angularVelocity = new Vector3(0.0f, 0.0f, 0.0f);
        other.transform.rotation = Quaternion.Euler(0.0f, 0.0f, 0.0f);
        other.transform.position = spawn;
    }
}
