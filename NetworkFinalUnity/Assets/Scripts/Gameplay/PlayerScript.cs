using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerScript : MonoBehaviour
{
    public GameObject player;
    public ulong playerID;
    public Color generatedColor;
    Color playerColor;
    // Start is called before the first frame update
    void Start()
    {
        playerID = GetComponent<NetworkedPlayerController>().NetworkObjectId;
        generatedColor = GeneratePlayerColor();
        var cubeRenderer = player.GetComponent<Renderer>();
        cubeRenderer.material.SetColor("_Color", generatedColor);
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    private Color GeneratePlayerColor() 
    {
        playerColor = new Color(Random.Range(0f, 1f), Random.Range(0f, 1f), Random.Range(0f, 1f));
        return playerColor;
    }
}
