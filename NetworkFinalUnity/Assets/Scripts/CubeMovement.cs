using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CubeMovement : MonoBehaviour
{
    private Vector3 offset;
    public GameObject player;
    public GameObject playerBounds;
    public GameObject playerCenter;
    public GameObject playerTop;
    public GameObject playerBottom;
    public GameObject playerLeft;
    public GameObject playerRight;

    public int step = 9;
    public float speed = 0.01f;

    private bool input = true;

    // Start is called before the first frame update
    void Start()
    {
        Vector3 pos = new Vector3(0.5f, 0.5f, 0.5f);
        GameObject newPlayerBounds = Instantiate(playerBounds, pos, Quaternion.identity);
        newPlayerBounds.name = "PlayerCenter";
        playerCenter = GameObject.Find("PlayerCenter");
        playerTop = GameObject.Find("PlayerCenter/PlayerTop");
        playerBottom = GameObject.Find("PlayerCenter/PlayerBottom");
        playerLeft = GameObject.Find("PlayerCenter/PlayerLeft");
        playerRight = GameObject.Find("PlayerCenter/PlayerRight");
    }

    // Update is called once per frame
    void Update()
    {
        if (input) 
        {
            if (Input.GetKey(KeyCode.UpArrow)) 
            {
                StartCoroutine("MoveUp");
                input = false;
            }
            if (Input.GetKey(KeyCode.DownArrow))
            {
                StartCoroutine("MoveDown");
                input = false;
            }
            if (Input.GetKey(KeyCode.LeftArrow))
            {
                StartCoroutine("MoveLeft");
                input = false;
            }
            if (Input.GetKey(KeyCode.RightArrow))
            {
                StartCoroutine("MoveRight");
                input = false;
            }
        }
    }

    IEnumerator MoveUp() 
    {
        for (int i = 0; i < (90 / step); i++) 
        {
            player.transform.RotateAround(playerTop.transform.position, Vector3.right, step);
            yield return new WaitForSeconds(speed);
        }
        playerCenter.transform.position = player.transform.position;

        input = true;
    }

    IEnumerator MoveDown()
    {
        for (int i = 0; i < (90 / step); i++)
        {
            player.transform.RotateAround(playerBottom.transform.position, Vector3.left, step);
            yield return new WaitForSeconds(speed);
        }
        playerCenter.transform.position = player.transform.position;

        input = true;
    }

    IEnumerator MoveLeft()
    {
        for (int i = 0; i < (90 / step); i++)
        {
            player.transform.RotateAround(playerLeft.transform.position, Vector3.forward, step);
            yield return new WaitForSeconds(speed);
        }
        playerCenter.transform.position = player.transform.position;

        input = true;
    }

    IEnumerator MoveRight()
    {
        for (int i = 0; i < (90 / step); i++)
        {
            player.transform.RotateAround(playerRight.transform.position, Vector3.back, step);
            yield return new WaitForSeconds(speed);
        }
        playerCenter.transform.position = player.transform.position;

        input = true;
    }
}
